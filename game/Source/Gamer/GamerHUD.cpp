#include "GamerHUD.h"

#include "GamerCharacter.h"
#include "GamerDataSubsystem.h"
#include "GamerInventoryComponent.h"
#include "GamerResourceNode.h"
#include "GamerWorkComponent.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/World.h"

namespace
{
	const FLinearColor TextWhite(0.95f, 0.95f, 0.92f);
	const FLinearColor TextDim(0.70f, 0.70f, 0.68f);
	const FLinearColor TextGood(0.45f, 0.90f, 0.45f);
	const FLinearColor TextBad(0.95f, 0.45f, 0.40f);
	const FLinearColor TextHint(1.00f, 0.85f, 0.40f);

	FString ToolNameShort(FName Type)
	{
		if (Type == TEXT("axe")) { return TEXT("eine Axt"); }
		if (Type == TEXT("pickaxe")) { return TEXT("eine Spitzhacke"); }
		if (Type == TEXT("hammer")) { return TEXT("einen Hammer"); }
		return Type.ToString();
	}

	FString ItemName(const UGamerDataSubsystem* Data, FName Id)
	{
		const FGamerItemDef* Def = Data ? Data->FindItem(Id) : nullptr;
		return Def ? Def->Name : Id.ToString();
	}
}

void AGamerHUD::DrawTextLine(const FString& Text, float X, float& Y, const FLinearColor& Color, float Scale)
{
	UFont* Font = GEngine ? GEngine->GetMediumFont() : nullptr;
	DrawText(Text, Color, X, Y, Font, Scale);
	Y += 20.f * Scale;
}

void AGamerHUD::DrawCentered(const FString& Text, float Y, const FLinearColor& Color)
{
	UFont* Font = GEngine ? GEngine->GetMediumFont() : nullptr;
	float Width = 0.f, Height = 0.f;
	GetTextSize(Text, Width, Height, Font);
	DrawText(Text, Color, (Canvas->ClipX - Width) * 0.5f, Y, Font);
}

void AGamerHUD::DrawHUD()
{
	Super::DrawHUD();

	const AGamerCharacter* Character = Cast<AGamerCharacter>(GetOwningPawn());
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (!Canvas || !Character || !Data)
	{
		return;
	}
	const UGamerInventoryComponent* Inventory = Character->GetInventory();
	const UGamerWorkComponent* Work = Character->GetWork();
	const float Width = Canvas->ClipX;
	const float Height = Canvas->ClipY;

	// Crosshair.
	DrawRect(FLinearColor(1.f, 1.f, 1.f, 0.8f), Width * 0.5f - 2.f, Height * 0.5f - 2.f, 4.f, 4.f);

	// Inventory, top left.
	float Y = 20.f;
	DrawTextLine(TEXT("Inventar"), 20.f, Y, TextWhite, 1.1f);
	bool bAny = false;
	for (const FGamerInventorySlot& Slot : Inventory->GetSlots())
	{
		if (!Slot.IsEmpty())
		{
			DrawTextLine(FString::Printf(TEXT("%s  x%d"), *ItemName(Data, Slot.Item), Slot.Count), 30.f, Y, TextDim);
			bAny = true;
		}
	}
	if (!bAny)
	{
		DrawTextLine(TEXT("(leer)"), 30.f, Y, TextDim);
	}
	DrawTextLine(FString::Printf(TEXT("Gewicht: %.1f kg"), Inventory->GetTotalWeightKg()), 30.f, Y, TextDim);

	// Gathering hint under the crosshair.
	if (const AGamerResourceNode* Node = Character->GetFocusNode())
	{
		if (const FGamerResourceNodeDef* Def = Node->GetDef())
		{
			FString Hint = FString::Printf(TEXT("E: %s"), *Def->Name);
			FLinearColor Color = TextHint;
			if (Def->RequiredTool.IsRequired() && !Inventory->FindBestTool(Def->RequiredTool.Type, Def->RequiredTool.MinTier))
			{
				Hint += FString::Printf(TEXT("  (braucht %s)"), *ToolNameShort(Def->RequiredTool.Type));
				Color = TextBad;
			}
			DrawCentered(Hint, Height * 0.5f + 20.f, Color);
		}
	}

	// Work progress.
	if (Work->IsBusy())
	{
		const float BarWidth = 260.f;
		const float BarX = (Width - BarWidth) * 0.5f;
		const float BarY = Height * 0.5f + 50.f;
		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.5f), BarX, BarY, BarWidth, 10.f);
		DrawRect(TextGood, BarX, BarY, BarWidth * Work->GetProgress(), 10.f);
		DrawCentered(Work->GetWork().Label, BarY + 14.f, TextWhite);
	}

	// Last feedback message for a few seconds.
	const double Now = GetWorld()->GetTimeSeconds();
	if (Now - Work->GetLastMessageTime() < 4.0)
	{
		DrawCentered(Work->GetLastMessage(), Height * 0.5f + 90.f, TextWhite);
	}

	// Start crafting menu, right side.
	if (Character->IsCraftMenuOpen())
	{
		const float X = Width - 520.f;
		float MenuY = 20.f;
		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.55f), X - 15.f, 10.f, 515.f, 40.f + 62.f * Character->GetHandRecipes().Num());
		DrawTextLine(TEXT("Herstellen (ohne Werkbank)   Tab schließt"), X, MenuY, TextWhite, 1.1f);
		int32 Number = 1;
		for (const FName RecipeId : Character->GetHandRecipes())
		{
			const FGamerRecipeDef* Recipe = Data->FindRecipe(RecipeId);
			if (!Recipe || Recipe->Outputs.Num() == 0)
			{
				continue;
			}
			const bool bCanCraft = Inventory->HasAll(Recipe->Inputs);
			DrawTextLine(FString::Printf(TEXT("%d   %s"), Number++, *ItemName(Data, Recipe->Outputs[0].Item)), X, MenuY, bCanCraft ? TextGood : TextWhite, 1.05f);
			FString Inputs;
			for (const FGamerItemAmount& Input : Recipe->Inputs)
			{
				Inputs += FString::Printf(TEXT("%s %d/%d   "), *ItemName(Data, Input.Item), Inventory->CountOf(Input.Item), Input.Count);
			}
			DrawTextLine(Inputs, X + 30.f, MenuY, bCanCraft ? TextDim : TextBad);
			MenuY += 18.f;
		}
	}

	// Controls, bottom left.
	float HelpY = Height - 30.f;
	DrawTextLine(TEXT("WASD laufen · Maus umsehen · Leertaste springen · E sammeln · Tab Herstellen · 1–9 auswählen"), 20.f, HelpY, TextDim, 0.9f);
}

#include "GamerHUD.h"

#include "GamerBuildComponent.h"
#include "GamerBuilding.h"
#include "GamerCharacter.h"
#include "GamerGameState.h"
#include "GamerGraphicsSettings.h"
#include "GamerDataSubsystem.h"
#include "GamerInventoryComponent.h"
#include "GamerKeyBindings.h"
#include "GamerMenuGameMode.h"
#include "GamerSessionSubsystem.h"
#include "GamerSettlement.h"
#include "GamerVillager.h"
#include "GamerPlayerController.h"
#include "GamerResourceNode.h"
#include "GamerWorkComponent.h"
#include "CanvasItem.h"
#include "EngineUtils.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"

namespace
{
	// No red: it was hard to read in the first play test (2026-11-13). Missing things are orange,
	// and the text says what is missing, so colour is never the only hint.
	const FLinearColor TextWhite(0.97f, 0.97f, 0.94f);
	const FLinearColor TextDim(0.82f, 0.82f, 0.80f);
	const FLinearColor TextGood(0.50f, 0.95f, 0.50f);
	const FLinearColor TextMissing(1.00f, 0.62f, 0.15f);
	const FLinearColor TextHint(1.00f, 0.88f, 0.45f);
	const FLinearColor PanelColor(0.f, 0.f, 0.f, 0.55f);

	TAutoConsoleVariable<float> CVarHudScale(
		TEXT("gamer.HudScale"), 1.f,
		TEXT("Extra size factor for the placeholder HUD text (1 = automatic size from the window height)."));

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

UFont* AGamerHUD::HudFont() const
{
	return GEngine ? GEngine->GetMediumFont() : nullptr;
}

float AGamerHUD::TextWidth(const FString& Text, float Scale)
{
	float Width = 0.f, Height = 0.f;
	GetTextSize(Text, Width, Height, HudFont(), Scale);
	return Width;
}

float AGamerHUD::LineHeight(float Scale)
{
	float Width = 0.f, Height = 0.f;
	GetTextSize(TEXT("Ag"), Width, Height, HudFont(), Scale);
	return Height * 1.12f;
}

void AGamerHUD::DrawShadowText(const FString& Text, float X, float Y, const FLinearColor& Color, float Scale)
{
	FCanvasTextItem Item(FVector2D(X, Y), FText::FromString(Text), HudFont(), Color);
	Item.Scale = FVector2D(Scale, Scale);
	Item.BlendMode = SE_BLEND_Translucent;
	const float Offset = FMath::Max(1.f, FMath::RoundToFloat(Scale));
	Item.EnableShadow(FLinearColor(0.f, 0.f, 0.f, 0.9f), FVector2D(Offset, Offset));
	Canvas->DrawItem(Item);
}

void AGamerHUD::DrawPanel(float X, float Y, float W, float H)
{
	DrawRect(PanelColor, X, Y, W, H);
}

FVector2D AGamerHUD::DrawLineBlock(const TArray<FGamerHudLine>& Lines, float X, float Y)
{
	const float Pad = 6.f * Ui;
	FVector2D Size(0.f, 0.f);
	for (const FGamerHudLine& Line : Lines)
	{
		Size.X = FMath::Max(Size.X, TextWidth(Line.Text, Ui * Line.Size));
		Size.Y += LineHeight(Ui * Line.Size);
	}
	DrawPanel(X - Pad, Y - Pad, Size.X + 2.f * Pad, Size.Y + 2.f * Pad);
	for (const FGamerHudLine& Line : Lines)
	{
		DrawShadowText(Line.Text, X, Y, Line.Color, Ui * Line.Size);
		Y += LineHeight(Ui * Line.Size);
	}
	return Size;
}

void AGamerHUD::DrawCentered(const FString& Text, float Y, const FLinearColor& Color, float Size)
{
	if (Text.IsEmpty())
	{
		return;
	}
	const float Scale = Ui * Size;
	const float Width = TextWidth(Text, Scale);
	const float X = (Canvas->ClipX - Width) * 0.5f;
	const float Pad = 5.f * Ui;
	DrawPanel(X - Pad, Y - Pad * 0.5f, Width + 2.f * Pad, LineHeight(Scale) + Pad);
	DrawShadowText(Text, X, Y, Color, Scale);
}

void AGamerHUD::DrawHUD()
{
	Super::DrawHUD();
	MenuButtons.Reset();
	ButtonTargets.Reset();
	const AGamerPlayerController* GamerPC = Cast<AGamerPlayerController>(PlayerOwner);
	if (Canvas && GamerPC && GamerPC->IsMenuOpen())
	{
		// The menu needs no pawn (e.g. while waiting for a respawn).
		Ui = FMath::Max(1.4f, Canvas->ClipY / 1080.f * 1.8f) * FMath::Clamp(CVarHudScale.GetValueOnGameThread(), 0.5f, 3.f) * GamerGraphics::GetHudScale();
		DrawMenu(*GamerPC);
		return;
	}

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

	// 1.4x the engine font in a small window, growing with the window height.
	Ui = FMath::Max(1.4f, Height / 1080.f * 1.8f) * FMath::Clamp(CVarHudScale.GetValueOnGameThread(), 0.5f, 3.f) * GamerGraphics::GetHudScale();
	const float Margin = 10.f * Ui;
	const float LineH = LineHeight(Ui);

	if (GamerPC && GamerPC->IsInBirdView())
	{
		DrawBirdView(*GamerPC);
		if (GetWorld()->GetTimeSeconds() - Work->GetLastMessageTime() < 4.0)
		{
			DrawCentered(Work->GetLastMessage(), Height * 0.5f - LineH * 3.0f, TextWhite);
		}
		return;
	}

	// Crosshair.
	const float Dot = FMath::Max(4.f, 3.f * Ui);
	DrawRect(FLinearColor(1.f, 1.f, 1.f, 0.85f), (Width - Dot) * 0.5f, (Height - Dot) * 0.5f, Dot, Dot);

	// Inventory overview, top left: totals per item (the hotbar shows the slots).
	{
		TArray<FGamerHudLine> Lines;
		Lines.Add({ TEXT("Inventar"), TextWhite, 1.05f });
		TArray<FName> Order;
		TMap<FName, int32> Totals;
		for (const FGamerInventorySlot& Slot : Inventory->GetSlots())
		{
			if (!Slot.IsEmpty())
			{
				if (!Totals.Contains(Slot.Item))
				{
					Order.Add(Slot.Item);
				}
				Totals.FindOrAdd(Slot.Item) += Slot.Count;
			}
		}
		for (const FName Item : Order)
		{
			Lines.Add({ FString::Printf(TEXT("%s  x%d"), *ItemName(Data, Item), Totals[Item]), TextDim, 0.95f });
		}
		if (Order.Num() == 0)
		{
			Lines.Add({ TEXT("(leer)"), TextDim, 0.95f });
		}
		Lines.Add({ FString::Printf(TEXT("Gewicht: %.1f kg"), Inventory->GetTotalWeightKg()), TextDim, 0.85f });
		DrawLineBlock(Lines, Margin, Margin);
	}

	// Hint under the crosshair. Trees and rocks: strike with the right tool in hand; the rest: E.
	const float BarY = Height * 0.5f + LineH * 3.0f;
	const float BarWidth = FMath::Min(Width * 0.4f, 180.f * Ui);
	const float BarHeight = 7.f * Ui;
	const float BarX = (Width - BarWidth) * 0.5f;
	if (const AGamerResourceNode* Node = Character->GetFocusNode())
	{
		if (const FGamerResourceNodeDef* Def = Node->GetDef())
		{
			const float HintY = Height * 0.5f + LineH * 0.8f;
			if (Def->RequiredTool.IsRequired())
			{
				DrawCentered(FString::Printf(TEXT("%s: %s abbauen"), *UGamerKeyBindings::ActionKeyName(GamerInputActions::Strike), *Def->Name), HintY, TextHint);
				const FName ToolType = Def->RequiredTool.Type;
				const int32 MinTier = Def->RequiredTool.MinTier;
				if (!Inventory->IsHoldingTool(ToolType, MinTier))
				{
					const int32 HotbarIndex = Inventory->FindToolInHotbar(ToolType, MinTier);
					const FString Missing = HotbarIndex != INDEX_NONE
						? FString::Printf(TEXT("%s: %s in die Hand nehmen"), *UGamerKeyBindings::ActionKeyName(GamerInputActions::Hotbar(HotbarIndex + 1)), *ItemName(Data, Inventory->GetSlots()[HotbarIndex].Item))
						: FString::Printf(TEXT("Braucht %s"), *ToolNameShort(ToolType));
					DrawCentered(Missing, HintY + LineH * 1.1f, TextMissing, 0.9f);
				}
				if (Node->GetWorkProgress() > 0.f && !Work->IsBusy())
				{
					DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.6f), BarX - 2.f, BarY - 2.f, BarWidth + 4.f, BarHeight + 4.f);
					DrawRect(TextHint, BarX, BarY, BarWidth * Node->GetWorkProgress(), BarHeight);
				}
			}
			else
			{
				DrawCentered(FString::Printf(TEXT("%s: %s sammeln"), *UGamerKeyBindings::ActionKeyName(GamerInputActions::Interact), *Def->Name), HintY, TextHint);
			}
		}
	}
	else if (const AGamerBuilding* FocusBuilding = Character->GetFocusBuilding())
	{
		if (const FGamerBuildableDef* Def = FocusBuilding->GetDef())
		{
			const float HintY = Height * 0.5f + LineH * 0.8f;
			if (!FocusBuilding->IsFinished())
			{
				DrawCentered(FString::Printf(TEXT("Bauplan: %s  (%d %%)"), *Def->Name, FMath::RoundToInt(FocusBuilding->GetProgress() * 100.f)), HintY, TextHint);
				DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.6f), BarX - 2.f, BarY - 2.f, BarWidth + 4.f, BarHeight + 4.f);
				DrawRect(TextHint, BarX, BarY, BarWidth * FocusBuilding->GetProgress(), BarHeight);
				if (AGamerGameState::Get(this).BuildFlow == EGamerBuildFlow::MaterialOnBuild)
				{
					TArray<FString> Parts;
					for (const FGamerItemAmount& Cost : Def->Cost)
					{
						const FGamerItemAmount* Spent = FocusBuilding->GetSpent().FindByPredicate([&Cost](const FGamerItemAmount& A) { return A.Item == Cost.Item; });
						Parts.Add(FString::Printf(TEXT("%s %d/%d"), *ItemName(Data, Cost.Item), Spent ? Spent->Count : 0, Cost.Count));
					}
					DrawCentered(TEXT("Verbaut: ") + FString::Join(Parts, TEXT(", ")), HintY + LineH * 1.1f, TextDim, 0.85f);
				}
			}
			else if (FocusBuilding->GetBuildableId() == TEXT("building_storage"))
			{
				DrawCentered(FString::Printf(TEXT("%s: Lager öffnen"), *UGamerKeyBindings::ActionKeyName(GamerInputActions::Interact)), HintY, TextHint);
			}
			else
			{
				DrawCentered(Def->Name, HintY, TextDim, 0.9f);
			}
		}
	}

	// Work progress (gathering by hand, crafting).
	if (Work->IsBusy())
	{
		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.6f), BarX - 2.f, BarY - 2.f, BarWidth + 4.f, BarHeight + 4.f);
		DrawRect(TextGood, BarX, BarY, BarWidth * Work->GetProgress(), BarHeight);
		DrawCentered(Work->GetWork().Label, BarY + BarHeight + 4.f * Ui, TextWhite, 0.9f);
	}

	// Short swing marker: the crosshair grows for a moment after each strike (no animation yet).
	const double Now = GetWorld()->GetTimeSeconds();
	if (Now - Character->GetLastStrikeTime() < 0.15)
	{
		const float Swing = Dot * 2.5f;
		DrawRect(FLinearColor(1.f, 1.f, 1.f, 0.5f), (Width - Swing) * 0.5f, (Height - Swing) * 0.5f, Swing, Swing);
	}

	// Last feedback message for a few seconds, above the crosshair.
	if (Now - Work->GetLastMessageTime() < 4.0)
	{
		DrawCentered(Work->GetLastMessage(), Height * 0.5f - LineH * 3.0f, TextWhite);
	}

	// Hotbar, bottom centre: the first inventory slots, keys 1-8.
	{
		const TArray<FGamerInventorySlot>& Slots = Inventory->GetSlots();
		const int32 Count = Inventory->GetHotbarSize();
		const float Gap = 4.f * Ui;
		const float Box = FMath::Min(52.f * Ui, (Width - 2.f * Margin - Gap * (Count - 1)) / FMath::Max(1, Count));
		const float TotalWidth = Count * Box + (Count - 1) * Gap;
		const float Y = Height - Margin - Box;
		float X = (Width - TotalWidth) * 0.5f;
		for (int32 Index = 0; Index < Count; ++Index, X += Box + Gap)
		{
			const bool bSelected = Index == Inventory->GetSelectedSlot();
			if (bSelected)
			{
				const float Border = FMath::Max(2.f, 2.f * Ui);
				DrawRect(TextHint, X - Border, Y - Border, Box + 2.f * Border, Box + 2.f * Border);
			}
			DrawRect(FLinearColor(0.f, 0.f, 0.f, bSelected ? 0.8f : 0.55f), X, Y, Box, Box);
			DrawShadowText(UGamerKeyBindings::ActionKeyName(GamerInputActions::Hotbar(Index + 1)), X + 3.f * Ui, Y + 1.f * Ui, bSelected ? TextHint : TextDim, Ui * 0.6f);

			const FGamerInventorySlot& Slot = Slots[Index];
			if (Slot.IsEmpty())
			{
				continue;
			}
			// Item name: shrink a little, then cut, so it fits the box. The full name is shown above the bar.
			FString Name = ItemName(Data, Slot.Item);
			float NameScale = Ui * 0.6f;
			const float Room = Box - 6.f * Ui;
			if (TextWidth(Name, NameScale) > Room)
			{
				NameScale = FMath::Max(Ui * 0.5f, NameScale * Room / TextWidth(Name, NameScale));
			}
			while (Name.Len() > 3 && TextWidth(Name, NameScale) > Room)
			{
				Name = Name.LeftChop(2) + TEXT(".");
			}
			DrawShadowText(Name, X + 3.f * Ui, Y + Box * 0.42f, TextWhite, NameScale);
			if (Slot.Count > 1)
			{
				const FString CountText = FString::Printf(TEXT("%d"), Slot.Count);
				const float CountScale = Ui * 0.6f;
				DrawShadowText(CountText, X + Box - 3.f * Ui - TextWidth(CountText, CountScale), Y + Box - LineHeight(CountScale), TextWhite, CountScale);
			}
		}
		if (const FGamerItemDef* Held = Inventory->GetSelectedItemDef())
		{
			DrawCentered(Held->Name, Y - LineH * 1.15f, TextWhite, 0.85f);
		}

		// Building with the hammer in hand: what a click does, the chosen piece and its cost (lines from the bottom up).
		if (Inventory->IsHoldingTool(TEXT("hammer"), 1))
		{
			const UGamerBuildComponent* BuildComp = Character->GetBuild();
			const FGamerWorldOptions& Options = AGamerGameState::Get(this);
			const FGamerBuildableDef* Piece = BuildComp->GetSelected().IsNone() ? nullptr : Data->FindBuildable(BuildComp->GetSelected());
			const AGamerBuilding* FocusBuilding = Character->GetFocusBuilding();
			const FString StrikeKey = UGamerKeyBindings::ActionKeyName(GamerInputActions::Strike);
			float InfoY = Y - LineH * 2.35f;
			auto InfoLine = [&](const FString& Text, const FLinearColor& Color)
			{
				DrawCentered(Text, InfoY, Color, 0.85f);
				InfoY -= LineH * 1.05f;
			};
			if (FocusBuilding && !FocusBuilding->IsFinished() && FocusBuilding->GetDef())
			{
				InfoLine(FString::Printf(TEXT("%s: %s weiterbauen"), *StrikeKey, *FocusBuilding->GetDef()->Name), TextHint);
			}
			else if (Piece)
			{
				const FGamerPlacement& Placement = BuildComp->GetPlacement();
				if (Placement.bValid)
				{
					InfoLine(FString::Printf(TEXT("%s: platzieren · %s: drehen · %s: anderes Bauteil"), *StrikeKey,
						*UGamerKeyBindings::ActionKeyName(GamerInputActions::Rotate), *UGamerKeyBindings::ActionKeyName(GamerInputActions::BuildMenu)), TextHint);
				}
				else
				{
					InfoLine(Placement.Problem.IsEmpty() ? FString(TEXT("Hier geht das nicht")) : Placement.Problem, TextMissing);
				}
			}
			if (Piece)
			{
				bool bEnough = true;
				TArray<FString> Parts;
				for (const FGamerItemAmount& Cost : Piece->Cost)
				{
					const int32 Have = Inventory->CountOf(Cost.Item);
					bEnough &= Have >= Cost.Count;
					Parts.Add(FString::Printf(TEXT("%s %d/%d"), *ItemName(Data, Cost.Item), Have, Cost.Count));
				}
				const bool bPayLater = Options.BuildFlow == EGamerBuildFlow::MaterialOnBuild;
				InfoLine(FString::Printf(TEXT("Bauteil: %s · %s%s"), *Piece->Name, bPayLater ? TEXT("Material beim Bauen: ") : TEXT("Kosten: "),
					*FString::Join(Parts, TEXT(", "))), bEnough || bPayLater ? TextWhite : TextMissing);
			}
			else if (!FocusBuilding || FocusBuilding->IsFinished())
			{
				InfoLine(FString::Printf(TEXT("%s: Bauteil wählen"), *UGamerKeyBindings::ActionKeyName(GamerInputActions::BuildMenu)), TextHint);
			}
			if (FocusBuilding && FocusBuilding->IsFinished() && FocusBuilding->GetDef() && Options.Demolish != EGamerDemolish::Disabled)
			{
				InfoLine(FString::Printf(TEXT("%s: %s abreißen"), *UGamerKeyBindings::ActionKeyName(GamerInputActions::Demolish), *FocusBuilding->GetDef()->Name), TextDim);
			}
		}
	}

	// Start crafting menu, top right. Measured at full size first; shrinks if wider than half the window.
	if (Character->IsCraftMenuOpen())
	{
		struct FMenuRow
		{
			FString Title;
			bool bCanCraft = false;
			TArray<TPair<FString, bool>> Inputs; // text, enough in inventory
		};
		TArray<FMenuRow> Rows;
		int32 Number = 1;
		for (const FName RecipeId : Character->GetHandRecipes())
		{
			const FGamerRecipeDef* Recipe = Data->FindRecipe(RecipeId);
			if (!Recipe || Recipe->Outputs.Num() == 0)
			{
				continue;
			}
			FMenuRow& Row = Rows.AddDefaulted_GetRef();
			Row.bCanCraft = Inventory->HasAll(Recipe->Inputs);
			const FString NumberKey = Number <= 8 ? UGamerKeyBindings::ActionKeyName(GamerInputActions::Hotbar(Number)) : FString::FromInt(Number);
			++Number;
			const FGamerBuildableDef* Station = Recipe->Station.IsNone() ? nullptr : Data->FindBuildable(Recipe->Station);
			Row.Title = FString::Printf(TEXT("%s   %s%s%s"), *NumberKey, *ItemName(Data, Recipe->Outputs[0].Item),
				Station ? *FString::Printf(TEXT("  [%s]"), *Station->Name) : TEXT(""), Row.bCanCraft ? TEXT("   (bereit)") : TEXT(""));
			for (const FGamerItemAmount& Input : Recipe->Inputs)
			{
				const int32 Have = Inventory->CountOf(Input.Item);
				Row.Inputs.Emplace(FString::Printf(TEXT("%s %d/%d"), *ItemName(Data, Input.Item), Have, Input.Count), Have >= Input.Count);
			}
		}

		const FString Header = FString::Printf(TEXT("Herstellen   (%s schließt)"), *UGamerKeyBindings::ActionKeyName(GamerInputActions::CraftMenu));
		const float TitleSize = 1.0f;
		const float InputSize = 0.85f;
		const float Gap = 1.5f;      // in line heights of the input text, scaled below
		auto MenuWidthAt = [&](float S)
		{
			float W = TextWidth(Header, S * 1.05f);
			for (const FMenuRow& Row : Rows)
			{
				W = FMath::Max(W, TextWidth(Row.Title, S * TitleSize));
				float InputW = 18.f * S;
				for (const TPair<FString, bool>& Input : Row.Inputs)
				{
					InputW += TextWidth(Input.Key, S * InputSize) + Gap * 8.f * S;
				}
				W = FMath::Max(W, InputW);
			}
			return W;
		};
		float S = Ui;
		const float FullWidth = MenuWidthAt(S);
		if (FullWidth > Width * 0.5f)
		{
			S *= Width * 0.5f / FullWidth;
		}
		const float MenuWidth = MenuWidthAt(S);
		const float Pad = 6.f * S;
		const float TitleH = LineHeight(S * TitleSize);
		const float InputH = LineHeight(S * InputSize);
		const float MenuHeight = LineHeight(S * 1.05f) * 1.3f + Rows.Num() * (TitleH + InputH + 0.35f * TitleH);
		const float X = Width - Margin - MenuWidth;
		float Y = Margin;
		DrawPanel(X - Pad, Y - Pad, MenuWidth + 2.f * Pad, MenuHeight + 2.f * Pad);
		DrawShadowText(Header, X, Y, TextWhite, S * 1.05f);
		Y += LineHeight(S * 1.05f) * 1.3f;
		for (const FMenuRow& Row : Rows)
		{
			DrawShadowText(Row.Title, X, Y, Row.bCanCraft ? TextGood : TextWhite, S * TitleSize);
			Y += TitleH;
			float InputX = X + 18.f * S;
			for (const TPair<FString, bool>& Input : Row.Inputs)
			{
				DrawShadowText(Input.Key, InputX, Y, Input.Value ? TextGood : TextMissing, S * InputSize);
				InputX += TextWidth(Input.Key, S * InputSize) + Gap * 8.f * S;
			}
			Y += InputH + 0.35f * TitleH;
		}
	}

	// Controls, top centre (hidden while the crafting menu is open, it has its own header).
	else
	{
		// Key names from the player's bindings ("WASD" when all four are single letters).
		const TArray<FString> MoveKeys = {
			UGamerKeyBindings::ActionKeyName(GamerInputActions::MoveForward), UGamerKeyBindings::ActionKeyName(GamerInputActions::MoveLeft),
			UGamerKeyBindings::ActionKeyName(GamerInputActions::MoveBack), UGamerKeyBindings::ActionKeyName(GamerInputActions::MoveRight) };
		const bool bShortMove = !MoveKeys.ContainsByPredicate([](const FString& Key) { return Key.Len() > 1; });
		const FString Move = FString::Printf(TEXT("%s laufen · Maus umsehen · %s springen · %s Vogelperspektive"),
			*FString::Join(MoveKeys, bShortMove ? TEXT("") : TEXT("/")), *UGamerKeyBindings::ActionKeyName(GamerInputActions::Jump),
			*UGamerKeyBindings::ActionKeyName(GamerInputActions::BirdView));
		const FString Act = FString::Printf(TEXT("%s schlagen · %s sammeln · %s–%s Schnellleiste · %s Herstellen · %s Dorf · Esc Menü"),
			*UGamerKeyBindings::ActionKeyName(GamerInputActions::Strike), *UGamerKeyBindings::ActionKeyName(GamerInputActions::Interact),
			*UGamerKeyBindings::ActionKeyName(GamerInputActions::Hotbar(1)), *UGamerKeyBindings::ActionKeyName(GamerInputActions::Hotbar(8)),
			*UGamerKeyBindings::ActionKeyName(GamerInputActions::CraftMenu), *UGamerKeyBindings::ActionKeyName(GamerInputActions::VillageMenu));
		const float HelpSize = 0.75f;
		TArray<FGamerHudLine> Lines;
		Lines.Add({ Move, TextDim, HelpSize });
		Lines.Add({ Act, TextDim, HelpSize });
		const float BlockWidth = FMath::Max(TextWidth(Move, Ui * HelpSize), TextWidth(Act, Ui * HelpSize));
		DrawLineBlock(Lines, (Width - BlockWidth) * 0.5f, Margin);
	}
}

// ---------------------------------------------------------------------------------------------
// In-game menu (Escape). Placeholder look like the rest of this HUD; the game keeps running.

FName AGamerHUD::FindMenuButtonAt(FVector2D Position) const
{
	for (const TPair<FName, FBox2D>& Button : MenuButtons)
	{
		if (Button.Value.IsInsideOrOn(Position))
		{
			return Button.Key;
		}
	}
	return NAME_None;
}

bool AGamerHUD::IsMouseOver(float X, float Y, float W, float H) const
{
	return MousePosition.X >= X && MousePosition.X <= X + W && MousePosition.Y >= Y && MousePosition.Y <= Y + H;
}

void AGamerHUD::DrawButton(FName Id, const FString& Text, float X, float Y, float W, float H, float Scale, bool bActive)
{
	const bool bHover = IsMouseOver(X, Y, W, H);
	const FLinearColor Fill = bActive ? FLinearColor(0.55f, 0.42f, 0.10f, 0.95f)
		: (bHover ? FLinearColor(0.32f, 0.32f, 0.30f, 0.95f) : FLinearColor(0.12f, 0.12f, 0.12f, 0.92f));
	if (bHover || bActive)
	{
		DrawRect(TextHint, X - 2.f, Y - 2.f, W + 4.f, H + 4.f);
	}
	DrawRect(Fill, X, Y, W, H);
	const float TextH = LineHeight(Scale) / 1.12f;
	DrawShadowText(Text, X + (W - TextWidth(Text, Scale)) * 0.5f, Y + (H - TextH) * 0.5f, TextWhite, Scale);
	MenuButtons.Emplace(Id, FBox2D(FVector2D(X, Y), FVector2D(X + W, Y + H)));
}

void AGamerHUD::DrawMenu(const AGamerPlayerController& PC)
{
	const float Width = Canvas->ClipX;
	const float Height = Canvas->ClipY;
	float MouseX = -1.f, MouseY = -1.f;
	MousePosition = PC.GetMousePosition(MouseX, MouseY) ? FVector2D(MouseX, MouseY) : FVector2D(-1.f, -1.f);

	DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.65f), 0.f, 0.f, Width, Height);

	if (PC.GetMenuPage() == EGamerMenuPage::Main)
	{
		const FString Title = TEXT("Menü");
		const TArray<TPair<FName, FString>> Buttons = {
			{ TEXT("resume"), TEXT("Weiter") },
			{ TEXT("controls"), TEXT("Steuerung") },
			{ TEXT("graphics"), TEXT("Grafik") },
			{ TEXT("leave"), TEXT("Hauptmenü") },
			{ TEXT("quit"), TEXT("Spiel beenden") },
		};
		const float S = Ui;
		float ButtonW = 0.f;
		for (const TPair<FName, FString>& Button : Buttons)
		{
			ButtonW = FMath::Max(ButtonW, TextWidth(Button.Value, S));
		}
		ButtonW += 60.f * S;
		const float ButtonH = LineHeight(S) * 1.6f;
		float Y = Height * 0.22f;
		DrawShadowText(Title, (Width - TextWidth(Title, S * 1.4f)) * 0.5f, Y, TextWhite, S * 1.4f);
		Y += LineHeight(S * 1.4f) * 1.6f;
		for (const TPair<FName, FString>& Button : Buttons)
		{
			DrawButton(Button.Key, Button.Value, (Width - ButtonW) * 0.5f, Y, ButtonW, ButtonH, S);
			Y += ButtonH + 10.f * S;
		}
		const FString Note = TEXT("Das Spiel läuft weiter (Koop).");
		DrawShadowText(Note, (Width - TextWidth(Note, S * 0.75f)) * 0.5f, Y + 6.f * S, TextDim, S * 0.75f);
		const FString World = TEXT("Welt: ") + AGamerGameState::Get(this).Describe();
		DrawShadowText(World, (Width - TextWidth(World, S * 0.7f)) * 0.5f, Y + 6.f * S + LineHeight(S * 0.75f) * 1.2f, TextDim, S * 0.7f);
		return;
	}

	if (PC.GetMenuPage() == EGamerMenuPage::Build)
	{
		DrawBuildMenu(PC);
		return;
	}
	if (PC.GetMenuPage() == EGamerMenuPage::Village)
	{
		DrawVillageMenu(PC);
		return;
	}
	if (PC.GetMenuPage() == EGamerMenuPage::Storage)
	{
		DrawStorageMenu(PC);
		return;
	}
	if (PC.GetMenuPage() == EGamerMenuPage::Graphics)
	{
		DrawGraphicsMenu(PC);
		return;
	}
	if (PC.GetMenuPage() == EGamerMenuPage::Start || PC.GetMenuPage() == EGamerMenuPage::CreateWorld || PC.GetMenuPage() == EGamerMenuPage::Join)
	{
		DrawStartMenu(PC);
		return;
	}

	// Controls page: two columns (actions | hotbar), each row = label + key box; click a row, press a key.
	const TArray<FGamerBindableAction>& Actions = UGamerKeyBindings::GetActions();
	auto IsHotbarAction = [](const FGamerBindableAction& Action) { return Action.Id.ToString().StartsWith(TEXT("hotbar_")); };
	int32 HotbarRows = 0;
	for (const FGamerBindableAction& Action : Actions)
	{
		HotbarRows += IsHotbarAction(Action) ? 1 : 0;
	}
	const int32 RowsPerColumn = FMath::Max(HotbarRows, Actions.Num() - HotbarRows);
	const FString Title = TEXT("Steuerung");
	const FString Help = TEXT("Aktion anklicken, dann die neue Taste drücken. Esc bricht ab. Controller-Tasten sind fest belegt.");
	const FString ResetText = TEXT("Standard wiederherstellen");
	const FString BackText = TEXT("Zurück");

	auto LayoutSize = [&](float S, float& OutColumnW, float& OutLabelW, float& OutKeyW, float& OutRowH)
	{
		OutLabelW = 0.f;
		OutKeyW = TextWidth(TEXT("Taste drücken …"), S * 0.9f);
		for (const FGamerBindableAction& Action : Actions)
		{
			OutLabelW = FMath::Max(OutLabelW, TextWidth(Action.Label, S * 0.9f));
			OutKeyW = FMath::Max(OutKeyW, TextWidth(UGamerKeyBindings::ActionKeyName(Action.Id), S * 0.9f));
		}
		OutKeyW += 24.f * S;
		OutColumnW = OutLabelW + 14.f * S + OutKeyW;
		OutRowH = LineHeight(S * 0.9f) * 1.3f;
		const float TotalW = FMath::Max(2.f * OutColumnW + 40.f * S, TextWidth(Help, S * 0.7f));
		const float TotalH = LineHeight(S * 1.3f) * 1.4f + LineHeight(S * 0.7f) * 1.6f + RowsPerColumn * (OutRowH + 4.f * S)
			+ LineHeight(S * 0.8f) * 1.6f + LineHeight(S) * 1.4f;
		return FVector2D(TotalW, TotalH);
	};

	float S = Ui;
	float ColumnW = 0.f, LabelW = 0.f, KeyW = 0.f, RowH = 0.f;
	const FVector2D Full = LayoutSize(S, ColumnW, LabelW, KeyW, RowH);
	S *= FMath::Min3(1.f, static_cast<float>(Width * 0.94f / Full.X), static_cast<float>(Height * 0.94f / Full.Y));
	const FVector2D Size = LayoutSize(S, ColumnW, LabelW, KeyW, RowH);
	const float Left = (Width - Size.X) * 0.5f;
	float Y = (Height - Size.Y) * 0.5f;

	DrawPanel(Left - 10.f * S, Y - 8.f * S, Size.X + 20.f * S, Size.Y + 16.f * S);
	DrawShadowText(Title, (Width - TextWidth(Title, S * 1.3f)) * 0.5f, Y, TextWhite, S * 1.3f);
	Y += LineHeight(S * 1.3f) * 1.4f;
	DrawShadowText(Help, (Width - TextWidth(Help, S * 0.7f)) * 0.5f, Y, TextDim, S * 0.7f);
	Y += LineHeight(S * 0.7f) * 1.6f;

	const float ColumnsX = (Width - (2.f * ColumnW + 40.f * S)) * 0.5f;
	const FName Listening = PC.GetListeningAction();
	const float RowTextH = LineHeight(S * 0.9f) / 1.12f;
	int32 RowInColumn[2] = { 0, 0 };
	for (int32 Index = 0; Index < Actions.Num(); ++Index)
	{
		const FGamerBindableAction& Action = Actions[Index];
		const int32 Column = IsHotbarAction(Action) ? 1 : 0;
		const float X = ColumnsX + Column * (ColumnW + 40.f * S);
		const float RowY = Y + (RowInColumn[Column]++) * (RowH + 4.f * S);
		DrawShadowText(Action.Label, X, RowY + (RowH - RowTextH) * 0.5f, TextWhite, S * 0.9f);
		const bool bListening = Listening == Action.Id;
		const FString KeyText = bListening ? TEXT("Taste drücken …") : UGamerKeyBindings::ActionKeyName(Action.Id);
		DrawButton(FName(*(TEXT("bind:") + Action.Id.ToString())), KeyText, X + LabelW + 14.f * S, RowY, KeyW, RowH, S * 0.9f, bListening);
	}
	Y += RowsPerColumn * (RowH + 4.f * S);

	const FString& Note = PC.GetMenuNote();
	if (!Note.IsEmpty())
	{
		DrawShadowText(Note, (Width - TextWidth(Note, S * 0.8f)) * 0.5f, Y + 4.f * S, TextHint, S * 0.8f);
	}
	Y += LineHeight(S * 0.8f) * 1.6f;

	const float ButtonH = LineHeight(S) * 1.4f;
	const float ResetW = TextWidth(ResetText, S) + 40.f * S;
	const float BackW = TextWidth(BackText, S) + 60.f * S;
	const float ButtonsX = (Width - (ResetW + 20.f * S + BackW)) * 0.5f;
	DrawButton(TEXT("reset"), ResetText, ButtonsX, Y, ResetW, ButtonH, S);
	DrawButton(TEXT("back"), BackText, ButtonsX + ResetW + 20.f * S, Y, BackW, ButtonH, S);
}

void AGamerHUD::DrawBuildMenu(const AGamerPlayerController& PC)
{
	// Build menu (hammer in hand + build key): building kit on the left, stations on the right.
	// Click a piece to choose it; the preview then follows the crosshair.
	const float Width = Canvas->ClipX;
	const float Height = Canvas->ClipY;
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const AGamerCharacter* Character = Cast<AGamerCharacter>(PC.GetPawn());
	const UGamerInventoryComponent* Inventory = Character ? Character->GetInventory() : nullptr;
	if (!Data || !Inventory)
	{
		return;
	}
	const FGamerWorldOptions& Options = AGamerGameState::Get(this);
	const bool bPayLater = Options.BuildFlow == EGamerBuildFlow::MaterialOnBuild;

	TArray<const FGamerBuildableDef*> Columns[2];
	for (const TPair<FName, FGamerBuildableDef>& Pair : Data->GetData().Buildables)
	{
		if (!Pair.Value.bDeprecated)
		{
			Columns[Pair.Value.Category == EGamerBuildCategory::Structure ? 0 : 1].Add(&Pair.Value);
		}
	}
	// Kit in data order is roughly bottom-up (foundation, walls, roof); keep it, stations by name.
	Columns[1].Sort([](const FGamerBuildableDef& A, const FGamerBuildableDef& B) { return A.Name < B.Name; });
	const int32 Rows = FMath::Max(Columns[0].Num(), Columns[1].Num());

	auto CostText = [&](const FGamerBuildableDef& Def, bool& bOutEnough)
	{
		bOutEnough = true;
		TArray<FString> Parts;
		for (const FGamerItemAmount& Cost : Def.Cost)
		{
			bOutEnough &= Inventory->CountOf(Cost.Item) >= Cost.Count;
			Parts.Add(FString::Printf(TEXT("%d %s"), Cost.Count, *ItemName(Data, Cost.Item)));
		}
		return FString::Join(Parts, TEXT(", "));
	};

	const FString Title = TEXT("Bauen");
	const FString Help = bPayLater
		? TEXT("Bauteil anklicken. Das Material wird erst beim Hämmern genommen.")
		: TEXT("Bauteil anklicken. Material: grün = vorhanden, orange = fehlt.");
	const FString WorldText = TEXT("Welt: ") + Options.Describe();
	const FString NoneText = TEXT("Kein Bauteil (nur hämmern)");
	const FString CloseText = TEXT("Schließen");

	auto Layout = [&](float S, float& OutNameW, float& OutCostW, float& OutRowH)
	{
		OutNameW = 0.f;
		OutCostW = 0.f;
		for (const TArray<const FGamerBuildableDef*>& Column : Columns)
		{
			for (const FGamerBuildableDef* Def : Column)
			{
				bool bEnough = true;
				OutNameW = FMath::Max(OutNameW, TextWidth(Def->Name, S * 0.85f));
				OutCostW = FMath::Max(OutCostW, TextWidth(CostText(*Def, bEnough), S * 0.7f));
			}
		}
		OutNameW += 24.f * S;
		OutRowH = LineHeight(S * 0.85f) * 1.25f;
		const float ColumnW = OutNameW + 10.f * S + OutCostW;
		const float TotalW = FMath::Max3(2.f * ColumnW + 40.f * S, TextWidth(Help, S * 0.7f), TextWidth(WorldText, S * 0.7f));
		const float TotalH = LineHeight(S * 1.3f) * 1.3f + LineHeight(S * 0.7f) * 2.6f + Rows * (OutRowH + 3.f * S) + LineHeight(S) * 1.8f;
		return FVector2D(TotalW, TotalH);
	};

	float S = Ui;
	float NameW = 0.f, CostW = 0.f, RowH = 0.f;
	const FVector2D Full = Layout(S, NameW, CostW, RowH);
	S *= FMath::Min3(1.f, static_cast<float>(Width * 0.96f / Full.X), static_cast<float>(Height * 0.94f / Full.Y));
	const FVector2D Size = Layout(S, NameW, CostW, RowH);
	float Y = (Height - Size.Y) * 0.5f;
	DrawPanel((Width - Size.X) * 0.5f - 10.f * S, Y - 8.f * S, Size.X + 20.f * S, Size.Y + 16.f * S);
	DrawShadowText(Title, (Width - TextWidth(Title, S * 1.3f)) * 0.5f, Y, TextWhite, S * 1.3f);
	Y += LineHeight(S * 1.3f) * 1.3f;
	DrawShadowText(Help, (Width - TextWidth(Help, S * 0.7f)) * 0.5f, Y, TextDim, S * 0.7f);
	Y += LineHeight(S * 0.7f) * 1.2f;
	DrawShadowText(WorldText, (Width - TextWidth(WorldText, S * 0.7f)) * 0.5f, Y, TextDim, S * 0.7f);
	Y += LineHeight(S * 0.7f) * 1.4f;

	const float ColumnW = NameW + 10.f * S + CostW;
	const float ColumnsX = (Width - (2.f * ColumnW + 40.f * S)) * 0.5f;
	const FName Selected = Character->GetBuild()->GetSelected();
	const float CostTextH = LineHeight(S * 0.7f) / 1.12f;
	for (int32 Column = 0; Column < 2; ++Column)
	{
		for (int32 Row = 0; Row < Columns[Column].Num(); ++Row)
		{
			const FGamerBuildableDef& Def = *Columns[Column][Row];
			const float X = ColumnsX + Column * (ColumnW + 40.f * S);
			const float RowY = Y + Row * (RowH + 3.f * S);
			DrawButton(FName(*(TEXT("build:") + Def.Id.ToString())), Def.Name, X, RowY, NameW, RowH, S * 0.85f, Def.Id == Selected);
			bool bEnough = true;
			const FString Cost = CostText(Def, bEnough);
			DrawShadowText(Cost, X + NameW + 10.f * S, RowY + (RowH - CostTextH) * 0.5f, bPayLater ? TextDim : (bEnough ? TextGood : TextMissing), S * 0.7f);
		}
	}
	Y += Rows * (RowH + 3.f * S) + 8.f * S;

	const float ButtonH = LineHeight(S) * 1.4f;
	const float NoneW = TextWidth(NoneText, S) + 40.f * S;
	const float CloseW = TextWidth(CloseText, S) + 60.f * S;
	const float ButtonsX = (Width - (NoneW + 20.f * S + CloseW)) * 0.5f;
	DrawButton(TEXT("build:none"), NoneText, ButtonsX, Y, NoneW, ButtonH, S, Selected.IsNone());
	DrawButton(TEXT("resume"), CloseText, ButtonsX + NoneW + 20.f * S, Y, CloseW, ButtonH, S);
}

void AGamerHUD::DrawStartMenu(const AGamerPlayerController& PC)
{
	// Start menu (map Entry): main page, "Welt erstellen" with the world options, "Beitreten" with the address.
	const float Width = Canvas->ClipX;
	const float Height = Canvas->ClipY;
	const float S = Ui;
	const float ButtonH = LineHeight(S) * 1.5f;
	const float Gap = 9.f * S;
	auto Centered = [&](const FString& Text, float Y, const FLinearColor& Color, float Size)
	{
		DrawShadowText(Text, (Width - TextWidth(Text, S * Size)) * 0.5f, Y, Color, S * Size);
	};

	// Darker background than the in-world menu: the start map is empty.
	DrawRect(FLinearColor(0.06f, 0.07f, 0.08f, 1.f), 0.f, 0.f, Width, Height);

	if (PC.GetMenuPage() == EGamerMenuPage::Start)
	{
		const TArray<TPair<FName, FString>> Buttons = {
			{ TEXT("create"), TEXT("Welt erstellen") },
			{ TEXT("joinpage"), TEXT("Beitreten") },
			{ TEXT("controls"), TEXT("Steuerung") },
			{ TEXT("graphics"), TEXT("Grafik") },
			{ TEXT("quit"), TEXT("Beenden") },
		};
		float ButtonW = 0.f;
		for (const TPair<FName, FString>& Button : Buttons)
		{
			ButtonW = FMath::Max(ButtonW, TextWidth(Button.Value, S));
		}
		ButtonW += 80.f * S;
		float Y = Height * 0.16f;
		Centered(TEXT("Gamer"), Y, TextWhite, 2.0f);
		Y += LineHeight(S * 2.f) * 1.1f;
		Centered(TEXT("Vertical Slice · Koop bis 8 Spieler"), Y, TextDim, 0.75f);
		Y += LineHeight(S * 0.75f) * 2.f;
		for (const TPair<FName, FString>& Button : Buttons)
		{
			DrawButton(Button.Key, Button.Value, (Width - ButtonW) * 0.5f, Y, ButtonW, ButtonH, S);
			Y += ButtonH + Gap;
		}
		const UGamerSessionSubsystem* Session = UGamerSessionSubsystem::Get(this);
		if (Session && !Session->GetLastError().IsEmpty())
		{
			Centered(Session->GetLastError(), Y + 6.f * S, TextMissing, 0.8f);
		}
		return;
	}

	if (PC.GetMenuPage() == EGamerMenuPage::CreateWorld)
	{
		const FGamerWorldOptions& Options = UGamerMenuSettings::Get()->WorldOptions;
		struct FOptionRow
		{
			FName Id;
			FString Label;
			FString Value;
			FString Explain;
		};
		const FOptionRow Rows[] = {
			{ TEXT("opt:flow"), TEXT("Bauablauf"),
				Options.BuildFlow == EGamerBuildFlow::Blueprint ? TEXT("Bauplan + Hammer") : Options.BuildFlow == EGamerBuildFlow::Instant ? TEXT("Sofort fertig") : TEXT("Material beim Bauen"),
				Options.BuildFlow == EGamerBuildFlow::Blueprint ? TEXT("Material beim Platzieren, dann fertig hämmern.")
					: Options.BuildFlow == EGamerBuildFlow::Instant ? TEXT("Material beim Platzieren, das Teil steht sofort.")
					: TEXT("Der Bauplan ist kostenlos, jeder Hammerschlag nimmt seinen Teil Material.") },
			{ TEXT("opt:place"), TEXT("Platzierung"),
				Options.Placement == EGamerPlacement::FreeSnap ? TEXT("Frei mit Einrasten") : TEXT("Festes Raster"),
				Options.Placement == EGamerPlacement::FreeSnap ? TEXT("Erstes Fundament frei, alles Weitere rastet am Vorhandenen ein.")
					: TEXT("Alles auf einem festen 3-m-Raster der Welt, Drehen in 90-Grad-Schritten.") },
			{ TEXT("opt:demolish"), TEXT("Abreißen"),
				Options.Demolish == EGamerDemolish::FullRefund ? TEXT("Volles Material zurück") : Options.Demolish == EGamerDemolish::HalfRefund ? TEXT("Halbes Material zurück") : TEXT("Aus"),
				Options.Demolish == EGamerDemolish::Disabled ? TEXT("Fertige Teile bleiben stehen. Unfertige Pläne geben immer alles zurück.")
					: TEXT("Gilt für fertige Teile. Unfertige Pläne geben immer alles zurück.") },
		};
		float LabelW = 0.f, ValueW = 0.f, ExplainW = 0.f;
		for (const FOptionRow& Row : Rows)
		{
			LabelW = FMath::Max(LabelW, TextWidth(Row.Label, S));
			ValueW = FMath::Max(ValueW, TextWidth(TEXT("Material beim Bauen"), S));
			ExplainW = FMath::Max(ExplainW, TextWidth(Row.Explain, S * 0.7f));
		}
		ValueW += 40.f * S;
		const float BlockW = FMath::Max(LabelW + 20.f * S + ValueW, ExplainW);
		const float X = (Width - BlockW) * 0.5f;
		float Y = Height * 0.12f;
		Centered(TEXT("Welt erstellen"), Y, TextWhite, 1.4f);
		Y += LineHeight(S * 1.4f) * 1.2f;
		Centered(TEXT("Wert anklicken, um ihn zu ändern. Gilt für alle Spieler dieser Welt."), Y, TextDim, 0.7f);
		Y += LineHeight(S * 0.7f) * 1.8f;
		for (const FOptionRow& Row : Rows)
		{
			const float TextH = LineHeight(S) / 1.12f;
			DrawShadowText(Row.Label, X, Y + (ButtonH - TextH) * 0.5f, TextWhite, S);
			DrawButton(Row.Id, Row.Value, X + LabelW + 20.f * S, Y, ValueW, ButtonH, S);
			Y += ButtonH + 3.f * S;
			DrawShadowText(Row.Explain, X, Y, TextDim, S * 0.7f);
			Y += LineHeight(S * 0.7f) * 1.5f;
		}
		Y += 4.f * S;
		Centered(TEXT("Du bist Gastgeber. Mitspieler treten über deine IP-Adresse bei (Port 7777)."), Y, TextDim, 0.7f);
		Y += LineHeight(S * 0.7f) * 1.6f;
		const FString StartText = TEXT("Welt starten");
		const FString BackText = TEXT("Zurück");
		const float StartW = TextWidth(StartText, S) + 60.f * S;
		const float BackW = TextWidth(BackText, S) + 60.f * S;
		const float ButtonsX = (Width - (StartW + 20.f * S + BackW)) * 0.5f;
		DrawButton(TEXT("start"), StartText, ButtonsX, Y, StartW, ButtonH, S, true);
		DrawButton(TEXT("back"), BackText, ButtonsX + StartW + 20.f * S, Y, BackW, ButtonH, S);
		Y += ButtonH + Gap;
		if (!PC.GetMenuNote().IsEmpty())
		{
			Centered(PC.GetMenuNote(), Y, TextHint, 0.8f);
		}
		return;
	}

	// Join page: the address field takes keys directly (digits, letters, ".", Shift+"." for ":", Backspace, Enter).
	float Y = Height * 0.18f;
	Centered(TEXT("Beitreten"), Y, TextWhite, 1.4f);
	Y += LineHeight(S * 1.4f) * 1.4f;
	Centered(TEXT("Adresse des Gastgebers"), Y, TextDim, 0.85f);
	Y += LineHeight(S * 0.85f) * 1.3f;
	const float FieldW = FMath::Min(Width * 0.7f, TextWidth(TEXT("000.000.000.000:00000"), S * 1.1f) + 60.f * S);
	const float FieldH = LineHeight(S * 1.1f) * 1.4f;
	const float FieldX = (Width - FieldW) * 0.5f;
	DrawRect(TextHint, FieldX - 2.f, Y - 2.f, FieldW + 4.f, FieldH + 4.f);
	DrawRect(FLinearColor(0.02f, 0.02f, 0.02f, 1.f), FieldX, Y, FieldW, FieldH);
	const bool bCaret = FMath::Fmod(GetWorld()->GetRealTimeSeconds(), 1.0) < 0.6;
	const FString Field = PC.GetJoinAddress() + (bCaret ? TEXT("|") : TEXT(" "));
	DrawShadowText(Field, FieldX + 12.f * S, Y + (FieldH - LineHeight(S * 1.1f) / 1.12f) * 0.5f, TextWhite, S * 1.1f);
	Y += FieldH + 10.f * S;
	Centered(TEXT("Zum Beispiel 192.168.0.12 im eigenen Netz. Anderer Port mit Doppelpunkt: 192.168.0.12:7778"), Y, TextDim, 0.7f);
	Y += LineHeight(S * 0.7f) * 1.2f;
	Centered(TEXT("Tippen, Rücktaste löscht, Eingabe verbindet. Auf diesem PC: 127.0.0.1"), Y, TextDim, 0.7f);
	Y += LineHeight(S * 0.7f) * 2.f;
	const FString ConnectText = TEXT("Verbinden");
	const FString BackText = TEXT("Zurück");
	const float ConnectW = TextWidth(ConnectText, S) + 60.f * S;
	const float BackW = TextWidth(BackText, S) + 60.f * S;
	const float ButtonsX = (Width - (ConnectW + 20.f * S + BackW)) * 0.5f;
	DrawButton(TEXT("connect"), ConnectText, ButtonsX, Y, ConnectW, ButtonH, S, true);
	DrawButton(TEXT("back"), BackText, ButtonsX + ConnectW + 20.f * S, Y, BackW, ButtonH, S);
	Y += ButtonH + Gap;
	if (!PC.GetMenuNote().IsEmpty())
	{
		Centered(PC.GetMenuNote(), Y, TextHint, 0.8f);
	}
}

AActor* AGamerHUD::FindButtonTarget(FName ButtonId) const
{
	const TWeakObjectPtr<AActor>* Target = ButtonTargets.Find(ButtonId);
	return Target ? Target->Get() : nullptr;
}

void AGamerHUD::DrawBar(float X, float Y, float W, float H, float Value)
{
	// Need bar: green when fine, yellow when getting low, orange when it needs care (no red, see the colours above).
	const float Clamped = FMath::Clamp(Value, 0.f, 1.f);
	const FLinearColor Fill = Clamped > 0.5f ? TextGood : Clamped > 0.25f ? TextHint : TextMissing;
	DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.7f), X, Y, W, H);
	DrawRect(Fill, X + 1.f, Y + 1.f, (W - 2.f) * Clamped, H - 2.f);
}

void AGamerHUD::DrawVillageMenu(const AGamerPlayerController& PC)
{
	// Village management: every villager with job (click to change), activity and needs.
	const float Width = Canvas->ClipX;
	const float Height = Canvas->ClipY;
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const AGamerSettlement* Settlement = AGamerSettlement::Get(this);
	const TArray<AGamerVillager*> Villagers = PC.GetVillagersSorted();

	const FString Title = TEXT("Dorfverwaltung");
	FString Info = TEXT("Noch keine Siedlung.");
	if (Settlement)
	{
		Info = FString::Printf(TEXT("Bewohner: %d   ·   freie Betten: %d"), Villagers.Num(), Settlement->GetFreeBeds());
		if (Settlement->GetFreeBeds() > 0)
		{
			Info += FString::Printf(TEXT(" (nächster Bewohner: %d %%)"), FMath::RoundToInt(Settlement->GetArrivalProgress() * 100.f));
		}
		Info += Settlement->GetCapacity() > 0
			? FString::Printf(TEXT("   ·   Lager: %d / %d"), Settlement->GetStoredTotal(), Settlement->GetCapacity())
			: FString(TEXT("   ·   noch kein Lager gebaut"));
	}
	const FString Help = TEXT("Beruf anklicken, um ihn zu wechseln. Geht der Beruf nicht, sammelt der Bewohner (Grund in der Tätigkeit).");
	const FString Empty = TEXT("Noch keine Bewohner. Baue ein Lager und Betten (Strohlager): Jedes freie Bett zieht einen Bewohner an.");

	float S = Ui;
	auto Layout = [&](float Scale, float& NameW, float& JobW, float& ActW, float& BarW, float& ShelterW, float& RowH)
	{
		NameW = TextWidth(TEXT("Torvald"), Scale * 0.85f) + 16.f * Scale;
		for (const AGamerVillager* Villager : Villagers)
		{
			NameW = FMath::Max(NameW, TextWidth(Villager->GetVillagerName(), Scale * 0.85f) + 16.f * Scale);
		}
		JobW = TextWidth(TEXT("Holzfäller"), Scale * 0.85f) + 30.f * Scale;
		ActW = TextWidth(TEXT("sammelt herabgefallene Äste – Holzfäller: keine Axt"), Scale * 0.65f);
		BarW = 60.f * Scale;
		ShelterW = TextWidth(TEXT("Unterkunft"), Scale * 0.65f) + 8.f * Scale;
		RowH = LineHeight(Scale * 0.85f) * 1.35f;
		const float TotalW = FMath::Max3(NameW + JobW + ActW + 2.f * BarW + ShelterW + 60.f * Scale, TextWidth(Help, Scale * 0.65f), TextWidth(Info, Scale * 0.75f));
		const int32 Rows = FMath::Max(1, Villagers.Num());
		const float TotalH = LineHeight(Scale * 1.3f) * 1.3f + LineHeight(Scale * 0.75f) * 1.4f + LineHeight(Scale * 0.65f) * 1.6f
			+ LineHeight(Scale * 0.65f) * 1.3f + Rows * (RowH + 3.f * Scale) + LineHeight(Scale) * 1.8f;
		return FVector2D(FMath::Max(TotalW, TextWidth(Empty, Scale * 0.7f)), TotalH);
	};
	float NameW, JobW, ActW, BarW, ShelterW, RowH;
	const FVector2D Full = Layout(S, NameW, JobW, ActW, BarW, ShelterW, RowH);
	S *= FMath::Min3(1.f, static_cast<float>(Width * 0.96f / Full.X), static_cast<float>(Height * 0.94f / Full.Y));
	const FVector2D Size = Layout(S, NameW, JobW, ActW, BarW, ShelterW, RowH);
	const float Left = (Width - Size.X) * 0.5f;
	float Y = (Height - Size.Y) * 0.5f;
	DrawPanel(Left - 10.f * S, Y - 8.f * S, Size.X + 20.f * S, Size.Y + 16.f * S);
	DrawShadowText(Title, (Width - TextWidth(Title, S * 1.3f)) * 0.5f, Y, TextWhite, S * 1.3f);
	Y += LineHeight(S * 1.3f) * 1.3f;
	DrawShadowText(Info, (Width - TextWidth(Info, S * 0.75f)) * 0.5f, Y, TextWhite, S * 0.75f);
	Y += LineHeight(S * 0.75f) * 1.4f;
	DrawShadowText(Help, (Width - TextWidth(Help, S * 0.65f)) * 0.5f, Y, TextDim, S * 0.65f);
	Y += LineHeight(S * 0.65f) * 1.6f;

	// Column positions.
	const float XName = Left;
	const float XJob = XName + NameW;
	const float XAct = XJob + JobW + 12.f * S;
	const float XHunger = XAct + ActW + 12.f * S;
	const float XSleep = XHunger + BarW + 12.f * S;
	const float XShelter = XSleep + BarW + 12.f * S;
	const float HeadScale = S * 0.65f;
	DrawShadowText(TEXT("Name"), XName, Y, TextDim, HeadScale);
	DrawShadowText(TEXT("Beruf"), XJob, Y, TextDim, HeadScale);
	DrawShadowText(TEXT("Tätigkeit"), XAct, Y, TextDim, HeadScale);
	DrawShadowText(TEXT("Hunger"), XHunger, Y, TextDim, HeadScale);
	DrawShadowText(TEXT("Schlaf"), XSleep, Y, TextDim, HeadScale);
	DrawShadowText(TEXT("Unterkunft"), XShelter, Y, TextDim, HeadScale);
	Y += LineHeight(HeadScale) * 1.3f;

	if (Villagers.Num() == 0)
	{
		DrawShadowText(Empty, (Width - TextWidth(Empty, S * 0.7f)) * 0.5f, Y + 4.f * S, TextHint, S * 0.7f);
		Y += RowH + 3.f * S;
	}
	const float TextH = LineHeight(S * 0.85f) / 1.12f;
	const float SmallH = LineHeight(S * 0.65f) / 1.12f;
	for (int32 Index = 0; Index < Villagers.Num(); ++Index)
	{
		const AGamerVillager* Villager = Villagers[Index];
		const FGamerJobDef* Job = Data ? Data->FindJob(Villager->GetJobId()) : nullptr;
		DrawShadowText(Villager->GetVillagerName(), XName, Y + (RowH - TextH) * 0.5f, TextWhite, S * 0.85f);
		const FName ButtonId(*FString::Printf(TEXT("job:%d"), Index));
		DrawButton(ButtonId, Job ? Job->Name : TEXT("-"), XJob, Y, JobW, RowH, S * 0.8f);
		ButtonTargets.Add(ButtonId, const_cast<AGamerVillager*>(Villager));
		FString Activity = Villager->GetActivity();
		while (Activity.Len() > 4 && TextWidth(Activity, S * 0.65f) > ActW)
		{
			Activity = Activity.LeftChop(2) + TEXT("…");
		}
		DrawShadowText(Activity, XAct, Y + (RowH - SmallH) * 0.5f, TextDim, S * 0.65f);
		const float BarH = RowH * 0.4f;
		DrawBar(XHunger, Y + (RowH - BarH) * 0.5f, BarW, BarH, Villager->GetHunger());
		DrawBar(XSleep, Y + (RowH - BarH) * 0.5f, BarW, BarH, Villager->GetSleep());
		const FString Shelter = !Villager->GetBed() ? TEXT("kein Bett") : Villager->HasShelter() ? TEXT("ja") : TEXT("kein Dach");
		DrawShadowText(Shelter, XShelter, Y + (RowH - SmallH) * 0.5f, Villager->HasShelter() ? TextGood : TextMissing, S * 0.65f);
		Y += RowH + 3.f * S;
	}

	Y += 8.f * S;
	const FString CloseText = TEXT("Schließen");
	const float CloseW = TextWidth(CloseText, S) + 60.f * S;
	DrawButton(TEXT("resume"), CloseText, (Width - CloseW) * 0.5f, Y, CloseW, LineHeight(S) * 1.4f, S);
}

void AGamerHUD::DrawGraphicsMenu(const AGamerPlayerController& PC)
{
	const float Width = Canvas->ClipX;
	const float Height = Canvas->ClipY;
	const TArray<FGamerGraphicsRow> Rows = GamerGraphics::GetRows();
	const FString Title = TEXT("Grafik");
	const FString Help = TEXT("Wert anklicken, um ihn zu ändern. Wirkt sofort und wird gespeichert.");
	const float Fps = GamerGraphics::GetAverageFps();
	const FString FpsText = Fps > 0.f ? FString::Printf(TEXT("Gerade: %.0f Bilder pro Sekunde"), Fps) : FString(TEXT(" "));
	const FString AutoText = TEXT("Automatisch einstellen");
	const FString DefaultText = TEXT("Standard");
	const FString BackText = TEXT("Zurück");

	auto Layout = [&](float S, float& LabelW, float& ValueW, float& RowH, float& ExplainH)
	{
		LabelW = 0.f;
		ValueW = TextWidth(TEXT("Vollbild (exklusiv)"), S * 0.9f);
		float ExplainW = 0.f;
		for (const FGamerGraphicsRow& Row : Rows)
		{
			LabelW = FMath::Max(LabelW, TextWidth(Row.Label, S * 0.9f));
			ValueW = FMath::Max(ValueW, TextWidth(Row.Value, S * 0.9f));
			ExplainW = FMath::Max(ExplainW, TextWidth(Row.Explain, S * 0.62f));
		}
		ValueW += 40.f * S;
		RowH = LineHeight(S * 0.9f) * 1.35f;
		ExplainH = LineHeight(S * 0.62f) * 1.35f;
		const float ButtonsW = TextWidth(AutoText, S * 0.85f) + TextWidth(DefaultText, S * 0.85f) + TextWidth(BackText, S * 0.85f) + 150.f * S + 32.f * S;
		const float TotalW = FMath::Max3(LabelW + 20.f * S + ValueW, ExplainW, FMath::Max(TextWidth(Help, S * 0.7f), ButtonsW));
		const float TotalH = LineHeight(S * 1.3f) * 1.3f + LineHeight(S * 0.7f) * 1.6f + Rows.Num() * (RowH + 2.f * S + ExplainH)
			+ LineHeight(S * 0.75f) * 1.5f + LineHeight(S * 0.85f) * 1.6f + LineHeight(S * 0.8f) * 1.6f;
		return FVector2D(TotalW, TotalH);
	};
	float S = Ui;
	float LabelW = 0.f, ValueW = 0.f, RowH = 0.f, ExplainH = 0.f;
	const FVector2D Full = Layout(S, LabelW, ValueW, RowH, ExplainH);
	S *= FMath::Min3(1.f, static_cast<float>(Width * 0.94f / Full.X), static_cast<float>(Height * 0.94f / Full.Y));
	const FVector2D Size = Layout(S, LabelW, ValueW, RowH, ExplainH);
	const float Left = (Width - Size.X) * 0.5f;
	float Y = (Height - Size.Y) * 0.5f;
	auto Centered = [&](const FString& Text, float Size, const FLinearColor& Color)
	{
		DrawShadowText(Text, (Width - TextWidth(Text, S * Size)) * 0.5f, Y, Color, S * Size);
	};

	DrawPanel(Left - 10.f * S, Y - 8.f * S, Size.X + 20.f * S, Size.Y + 16.f * S);
	Centered(Title, 1.3f, TextWhite);
	Y += LineHeight(S * 1.3f) * 1.3f;
	Centered(Help, 0.7f, TextDim);
	Y += LineHeight(S * 0.7f) * 1.6f;

	const float TextH = LineHeight(S * 0.9f) / 1.12f;
	for (const FGamerGraphicsRow& Row : Rows)
	{
		DrawShadowText(Row.Label, Left, Y + (RowH - TextH) * 0.5f, Row.bEnabled ? TextWhite : TextDim, S * 0.9f);
		const float ValueX = Left + LabelW + 20.f * S;
		if (Row.bEnabled)
		{
			DrawButton(Row.Id, Row.Value, ValueX, Y, ValueW, RowH, S * 0.9f);
		}
		else
		{
			DrawShadowText(Row.Value, ValueX + 10.f * S, Y + (RowH - TextH) * 0.5f, TextDim, S * 0.9f);
		}
		Y += RowH + 2.f * S;
		DrawShadowText(Row.Explain, Left, Y, TextDim, S * 0.62f);
		Y += ExplainH;
	}

	Y += LineHeight(S * 0.75f) * 0.2f;
	Centered(FpsText, 0.75f, TextGood);
	Y += LineHeight(S * 0.75f) * 1.3f;

	const float ButtonH = LineHeight(S * 0.85f) * 1.4f;
	const float AutoW = TextWidth(AutoText, S * 0.85f) + 50.f * S;
	const float DefaultW = TextWidth(DefaultText, S * 0.85f) + 50.f * S;
	const float BackW = TextWidth(BackText, S * 0.85f) + 50.f * S;
	float ButtonX = (Width - (AutoW + DefaultW + BackW + 32.f * S)) * 0.5f;
	DrawButton(TEXT("gfx:auto"), AutoText, ButtonX, Y, AutoW, ButtonH, S * 0.85f);
	ButtonX += AutoW + 16.f * S;
	DrawButton(TEXT("gfx:defaults"), DefaultText, ButtonX, Y, DefaultW, ButtonH, S * 0.85f);
	ButtonX += DefaultW + 16.f * S;
	DrawButton(TEXT("back"), BackText, ButtonX, Y, BackW, ButtonH, S * 0.85f);
	Y += ButtonH + 8.f * S;
	if (!PC.GetMenuNote().IsEmpty())
	{
		Centered(PC.GetMenuNote(), 0.75f, TextHint);
	}
}

void AGamerHUD::DrawBirdView(const AGamerPlayerController& PC)
{
	const float Width = Canvas->ClipX;
	const float Height = Canvas->ClipY;
	float MouseX = -1.f, MouseY = -1.f;
	MousePosition = PC.GetMousePosition(MouseX, MouseY) ? FVector2D(MouseX, MouseY) : FVector2D(-1.f, -1.f);
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const AActor* Selected = PC.GetBirdSelection();

	// Name and job over every villager in view; the selected one in yellow.
	for (TActorIterator<AGamerVillager> It(GetWorld()); It; ++It)
	{
		FVector2D OnScreen;
		if (!PC.ProjectWorldLocationToScreen(It->GetActorLocation() + FVector(0.f, 0.f, 120.f), OnScreen, true)
			|| OnScreen.X < 0.f || OnScreen.Y < 0.f || OnScreen.X > Width || OnScreen.Y > Height)
		{
			continue;
		}
		const FGamerJobDef* Job = Data ? Data->FindJob(It->GetJobId()) : nullptr;
		const FString Tag = Job ? FString::Printf(TEXT("%s · %s"), *It->GetVillagerName(), *Job->Name) : It->GetVillagerName();
		const float S = Ui * 0.7f;
		const float TagW = TextWidth(Tag, S);
		const float TagH = LineHeight(S);
		const float X = OnScreen.X - TagW * 0.5f;
		const float Y = OnScreen.Y - TagH;
		DrawPanel(X - 4.f * Ui, Y - 2.f * Ui, TagW + 8.f * Ui, TagH + 4.f * Ui);
		DrawShadowText(Tag, X, Y, *It == Selected ? TextHint : TextWhite, S);
	}
	// The name over a selected building or resource node, in yellow like a selected villager.
	const TArray<FString> Info = PC.DescribeBirdSelection();
	if (Selected && !Cast<AGamerVillager>(Selected) && Info.Num() > 0)
	{
		FVector Origin, Extent;
		Selected->GetActorBounds(true, Origin, Extent);
		FVector2D OnScreen;
		if (PC.ProjectWorldLocationToScreen(Origin + FVector(0.f, 0.f, Extent.Z + 60.f), OnScreen, true))
		{
			const float S = Ui * 0.75f;
			const float TagW = TextWidth(Info[0], S);
			DrawPanel(OnScreen.X - TagW * 0.5f - 4.f * Ui, OnScreen.Y - LineHeight(S) - 2.f * Ui, TagW + 8.f * Ui, LineHeight(S) + 4.f * Ui);
			DrawShadowText(Info[0], OnScreen.X - TagW * 0.5f, OnScreen.Y - LineHeight(S), TextHint, S);
		}
	}

	// Help, top centre.
	const FString Back = UGamerKeyBindings::ActionKeyName(GamerInputActions::BirdView);
	DrawCentered(FString::Printf(TEXT("Vogelperspektive  ·  %s oder Esc: zurück"), *Back), 10.f * Ui, TextHint, 0.85f);
	DrawCentered(FString::Printf(TEXT("%s %s %s %s: bewegen  ·  Mausrad: Zoom  ·  rechte Maustaste halten: drehen  ·  Linksklick: auswählen  ·  %s: Dorfverwaltung"),
		*UGamerKeyBindings::ActionKeyName(GamerInputActions::MoveForward), *UGamerKeyBindings::ActionKeyName(GamerInputActions::MoveLeft),
		*UGamerKeyBindings::ActionKeyName(GamerInputActions::MoveBack), *UGamerKeyBindings::ActionKeyName(GamerInputActions::MoveRight),
		*UGamerKeyBindings::ActionKeyName(GamerInputActions::VillageMenu)), 10.f * Ui + LineHeight(Ui * 0.85f) * 1.3f, TextDim, 0.65f);

	// Info panel of the selection, bottom left.
	const float Margin = 12.f * Ui;
	if (Info.Num() == 0)
	{
		const FString Hint = TEXT("Bewohner, Gebäude oder Pflanze anklicken für Infos");
		DrawPanel(Margin - 6.f * Ui, Height - Margin - LineHeight(Ui * 0.75f) - 6.f * Ui, TextWidth(Hint, Ui * 0.75f) + 12.f * Ui, LineHeight(Ui * 0.75f) + 12.f * Ui);
		DrawShadowText(Hint, Margin, Height - Margin - LineHeight(Ui * 0.75f), TextDim, Ui * 0.75f);
		return;
	}
	TArray<FGamerHudLine> Lines;
	for (int32 Index = 0; Index < Info.Num(); ++Index)
	{
		Lines.Add({ Info[Index], Index == 0 ? TextWhite : TextDim, Index == 0 ? 1.0f : 0.8f });
	}
	const bool bVillager = Cast<AGamerVillager>(Selected) != nullptr;
	const FString JobText = TEXT("Beruf wechseln");
	const float ButtonH = LineHeight(Ui * 0.8f) * 1.35f;
	float BlockH = 0.f;
	for (const FGamerHudLine& Line : Lines)
	{
		BlockH += LineHeight(Ui * Line.Size);
	}
	const float ExtraH = bVillager ? ButtonH + 8.f * Ui : 0.f;
	const float Top = Height - Margin - BlockH - ExtraH;
	const FVector2D Size = DrawLineBlock(Lines, Margin, Top);
	if (bVillager)
	{
		const float ButtonW = FMath::Max(Size.X, TextWidth(JobText, Ui * 0.8f) + 30.f * Ui);
		DrawButton(TEXT("birdjob"), JobText, Margin, Top + BlockH + 8.f * Ui, ButtonW, ButtonH, Ui * 0.8f);
		ButtonTargets.Add(TEXT("birdjob"), const_cast<AActor*>(Selected));
	}
}

void AGamerHUD::DrawStorageMenu(const AGamerPlayerController& PC)
{
	// Settlement storage: left what is stored (click = take one stack), right the own inventory (click = put all of it).
	const float Width = Canvas->ClipX;
	const float Height = Canvas->ClipY;
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const AGamerSettlement* Settlement = AGamerSettlement::Get(this);
	const AGamerCharacter* Character = Cast<AGamerCharacter>(PC.GetPawn());
	if (!Data || !Settlement || !Character)
	{
		return;
	}
	TArray<FGamerItemAmount> Stored = Settlement->GetStorage();
	Stored.Sort([&Data](const FGamerItemAmount& A, const FGamerItemAmount& B) { return ItemName(Data, A.Item) < ItemName(Data, B.Item); });
	TMap<FName, int32> Own;
	for (const FGamerInventorySlot& Slot : Character->GetInventory()->GetSlots())
	{
		if (!Slot.IsEmpty())
		{
			Own.FindOrAdd(Slot.Item) += Slot.Count;
		}
	}
	TArray<FName> OwnItems;
	Own.GetKeys(OwnItems);
	OwnItems.Sort([&Data](const FName& A, const FName& B) { return ItemName(Data, A) < ItemName(Data, B); });

	const FString Title = FString::Printf(TEXT("Lager  (%d / %d)"), Settlement->GetStoredTotal(), Settlement->GetCapacity());
	const FString Help = TEXT("Links anklicken: einen Stapel nehmen. Rechts anklicken: alles davon einlagern.");
	const int32 Rows = FMath::Max3(1, Stored.Num(), OwnItems.Num());

	float S = Ui;
	const float FullH = LineHeight(S * 1.3f) * 1.3f + LineHeight(S * 0.65f) * 1.5f + LineHeight(S * 0.8f) * 1.3f
		+ Rows * (LineHeight(S * 0.8f) * 1.3f + 3.f * S) + LineHeight(S) * 1.8f;
	S *= FMath::Min(1.f, Height * 0.94f / FullH);
	const float ColumnW = FMath::Max(TextWidth(TEXT("einfache Mahlzeit  9999"), S * 0.8f) + 30.f * S, 180.f * S);
	const float TotalW = FMath::Max(2.f * ColumnW + 40.f * S, TextWidth(Help, S * 0.65f));
	const float RowH = LineHeight(S * 0.8f) * 1.3f;
	const float TotalH = LineHeight(S * 1.3f) * 1.3f + LineHeight(S * 0.65f) * 1.5f + LineHeight(S * 0.8f) * 1.3f + Rows * (RowH + 3.f * S) + LineHeight(S) * 1.8f;
	const float Left = (Width - TotalW) * 0.5f;
	float Y = (Height - TotalH) * 0.5f;
	DrawPanel(Left - 10.f * S, Y - 8.f * S, TotalW + 20.f * S, TotalH + 16.f * S);
	DrawShadowText(Title, (Width - TextWidth(Title, S * 1.3f)) * 0.5f, Y, TextWhite, S * 1.3f);
	Y += LineHeight(S * 1.3f) * 1.3f;
	DrawShadowText(Help, (Width - TextWidth(Help, S * 0.65f)) * 0.5f, Y, TextDim, S * 0.65f);
	Y += LineHeight(S * 0.65f) * 1.5f;

	const float XLeft = (Width - (2.f * ColumnW + 40.f * S)) * 0.5f;
	const float XRight = XLeft + ColumnW + 40.f * S;
	DrawShadowText(TEXT("Im Lager"), XLeft, Y, TextWhite, S * 0.8f);
	DrawShadowText(TEXT("Dein Inventar"), XRight, Y, TextWhite, S * 0.8f);
	Y += LineHeight(S * 0.8f) * 1.3f;
	if (Stored.Num() == 0)
	{
		DrawShadowText(TEXT("(leer)"), XLeft, Y, TextDim, S * 0.8f);
	}
	for (int32 Index = 0; Index < Stored.Num(); ++Index)
	{
		const FGamerItemAmount& Amount = Stored[Index];
		DrawButton(FName(*(TEXT("take:") + Amount.Item.ToString())), FString::Printf(TEXT("%s  %d"), *ItemName(Data, Amount.Item), Amount.Count),
			XLeft, Y + Index * (RowH + 3.f * S), ColumnW, RowH, S * 0.8f);
	}
	if (OwnItems.Num() == 0)
	{
		DrawShadowText(TEXT("(leer)"), XRight, Y, TextDim, S * 0.8f);
	}
	for (int32 Index = 0; Index < OwnItems.Num(); ++Index)
	{
		const FName Item = OwnItems[Index];
		DrawButton(FName(*(TEXT("put:") + Item.ToString())), FString::Printf(TEXT("%s  %d"), *ItemName(Data, Item), Own[Item]),
			XRight, Y + Index * (RowH + 3.f * S), ColumnW, RowH, S * 0.8f);
	}
	Y += Rows * (RowH + 3.f * S) + 8.f * S;

	const FString PutAllText = TEXT("Alles außer Werkzeug einlagern");
	const FString CloseText = TEXT("Schließen");
	const float ButtonH = LineHeight(S) * 1.4f;
	const float PutAllW = TextWidth(PutAllText, S) + 40.f * S;
	const float CloseW = TextWidth(CloseText, S) + 60.f * S;
	const float ButtonsX = (Width - (PutAllW + 20.f * S + CloseW)) * 0.5f;
	DrawButton(TEXT("putall"), PutAllText, ButtonsX, Y, PutAllW, ButtonH, S);
	DrawButton(TEXT("resume"), CloseText, ButtonsX + PutAllW + 20.f * S, Y, CloseW, ButtonH, S);
}

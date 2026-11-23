// Inventory window (key I): the 24 slots as a grid, the first 8 are the hotbar. Click a slot to pick it up, click another to move,
// swap or merge; "Wegwerfen" throws the picked-up stack onto the ground. Own file so GamerHUD.cpp stays small.
#include "GamerHUD.h"

#include "GamerCharacter.h"
#include "GamerDataSubsystem.h"
#include "GamerInventoryComponent.h"
#include "GamerKeyBindings.h"
#include "GamerPlayerController.h"
#include "Engine/Canvas.h"

namespace
{
	FString InvItemName(const UGamerDataSubsystem* Data, FName Id)
	{
		const FGamerItemDef* Def = Data ? Data->FindItem(Id) : nullptr;
		return Def ? Def->Name : Id.ToString();
	}

	FLinearColor InvHex(uint32 Rgb, float Alpha = 1.f)
	{
		auto Channel = [](uint32 Value) { return FMath::Pow(static_cast<float>(Value) / 255.f, 2.2f); };
		return FLinearColor(Channel((Rgb >> 16) & 0xFF), Channel((Rgb >> 8) & 0xFF), Channel(Rgb & 0xFF), Alpha);
	}
}

void AGamerHUD::DrawInventoryMenu(const AGamerPlayerController& PC)
{
	const AGamerCharacter* Character = Cast<AGamerCharacter>(PC.GetPawn());
	const UGamerInventoryComponent* Inventory = Character ? Character->GetInventory() : nullptr;
	if (!Inventory || !Canvas)
	{
		return;
	}
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const float Width = Canvas->ClipX;
	const float Height = Canvas->ClipY;
	const TArray<FGamerInventorySlot>& Slots = Inventory->GetSlots();
	const int32 Columns = 6;
	const int32 Rows = FMath::DivideAndRoundUp(Slots.Num(), Columns);
	const FLinearColor White(0.97f, 0.97f, 0.94f);
	const FLinearColor Dim(0.82f, 0.82f, 0.80f);

	// Scale so the whole window always fits (width and height).
	float S = Ui;
	const float CellBase = 54.f;
	const float PanelWBase = Columns * (CellBase + 6.f) + 40.f;
	const float PanelHBase = 150.f + Rows * (CellBase + 6.f) + 70.f;
	S *= FMath::Min(1.f, FMath::Min(Width * 0.92f / (PanelWBase * Ui), Height * 0.92f / (PanelHBase * Ui)));
	const float Cell = CellBase * S;
	const float Gap = 6.f * S;
	const float PanelW = Columns * (Cell + Gap) + 40.f * S;
	const float PanelH = 150.f * S + Rows * (Cell + Gap) + 70.f * S;
	const float Left = (Width - PanelW) * 0.5f;
	float Y = (Height - PanelH) * 0.5f;
	DrawPanel(Left, Y, PanelW, PanelH);
	Y += 12.f * S;
	const FString Title = TEXT("Inventar");
	DrawShadowText(Title, (Width - TextWidth(Title, S * 1.3f)) * 0.5f, Y, White, S * 1.3f);
	Y += LineHeight(S * 1.3f) * 1.2f;
	const int32 Selected = PC.GetInventorySelected();
	const FString Help = Selected == INDEX_NONE ? TEXT("Platz anklicken zum Aufnehmen, dann einen anderen anklicken zum Verschieben oder Tauschen.")
		: TEXT("Jetzt den Zielplatz anklicken, den Platz nochmal zum Loslassen, oder wegwerfen.");
	DrawShadowText(Help, (Width - TextWidth(Help, S * 0.6f)) * 0.5f, Y, Dim, S * 0.6f);
	Y += LineHeight(S * 0.6f) * 1.6f;

	const float GridX = Left + 20.f * S;
	for (int32 Index = 0; Index < Slots.Num(); ++Index)
	{
		const int32 Col = Index % Columns;
		const int32 Row = Index / Columns;
		const float X = GridX + Col * (Cell + Gap);
		const float CY = Y + Row * (Cell + Gap);
		const bool bHotbar = Index < Inventory->GetHotbarSize();
		const bool bPicked = Index == Selected;
		// Frame: ocker for the hotbar, bright for the picked-up slot.
		DrawRect(bPicked ? InvHex(0xE2D8BC, 0.95f) : (bHotbar ? InvHex(0xA9863A, 0.85f) : InvHex(0x5B6360, 0.7f)), X - 2.f, CY - 2.f, Cell + 4.f, Cell + 4.f);
		DrawRect(InvHex(0x262B28, 0.95f), X, CY, Cell, Cell);
		FName ButtonId(*FString::Printf(TEXT("slot:%d"), Index));
		const FGamerInventorySlot& Slot = Slots[Index];
		if (!Slot.IsEmpty())
		{
			DrawIconTile(Slot.Item, InvItemName(Data, Slot.Item), X, CY, Cell);
			if (Slot.Count > 1)
			{
				const FString Count = FString::FromInt(Slot.Count);
				DrawShadowText(Count, X + Cell - TextWidth(Count, S * 0.7f) - 3.f * S, CY + Cell - LineHeight(S * 0.7f), White, S * 0.7f);
			}
		}
		if (bHotbar)
		{
			DrawShadowText(FString::FromInt(Index + 1), X + 3.f * S, CY + 1.f * S, Dim, S * 0.55f);
		}
		MenuButtons.Emplace(ButtonId, FBox2D(FVector2D(X, CY), FVector2D(X + Cell, CY + Cell)));
	}
	Y += Rows * (Cell + Gap) + 4.f * S;

	// What is picked up, and the buttons.
	FString Info = TEXT("Schnellleiste = die ersten 8 Plätze (Zahlentasten 1–8)");
	if (Selected != INDEX_NONE && Slots.IsValidIndex(Selected) && !Slots[Selected].IsEmpty() && Data)
	{
		Info = FString::Printf(TEXT("%s  x%d"), *InvItemName(Data, Slots[Selected].Item), Slots[Selected].Count);
	}
	DrawShadowText(Info, (Width - TextWidth(Info, S * 0.8f)) * 0.5f, Y, White, S * 0.8f);
	Y += LineHeight(S * 0.8f) * 1.4f;
	const FString DropText = TEXT("Wegwerfen");
	const FString DropOneText = TEXT("1 wegwerfen");
	const FString CloseText = FString::Printf(TEXT("Schließen (%s)"), *UGamerKeyBindings::ActionKeyName(GamerInputActions::InventoryMenu));
	const float ButtonH = LineHeight(S) * 1.5f;
	const float DropW = TextWidth(DropText, S) + 40.f * S;
	const float DropOneW = TextWidth(DropOneText, S) + 40.f * S;
	const float CloseW = TextWidth(CloseText, S) + 40.f * S;
	const float BX = (Width - (DropW + DropOneW + CloseW + 40.f * S)) * 0.5f;
	if (Selected != INDEX_NONE)
	{
		DrawButton(TEXT("invdrop"), DropText, BX, Y, DropW, ButtonH, S);
		DrawButton(TEXT("invdrop1"), DropOneText, BX + DropW + 20.f * S, Y, DropOneW, ButtonH, S);
	}
	DrawButton(TEXT("resume"), CloseText, BX + DropW + DropOneW + 40.f * S, Y, CloseW, ButtonH, S);
}

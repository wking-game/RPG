// Character pages of the start menu (origin, name, values, equipment; appearance editor).
// Part of AGamerHUD, kept in its own file so GamerHUD.cpp stays manageable. Placeholder look like the rest of the HUD.

#include "GamerHUD.h"

#include "GamerDataSubsystem.h"
#include "GamerPlayerController.h"
#include "GamerPlayerState.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"

namespace
{
	const FLinearColor CharWhite(0.97f, 0.97f, 0.94f);
	const FLinearColor CharDim(0.82f, 0.82f, 0.80f);
	const FLinearColor CharHint(1.00f, 0.88f, 0.45f);
	const FLinearColor CharGood(0.50f, 0.95f, 0.50f);
}

TArray<FString> AGamerHUD::WrapText(const FString& Text, float MaxWidth, float Scale)
{
	TArray<FString> Words;
	Text.ParseIntoArray(Words, TEXT(" "));
	TArray<FString> Lines;
	FString Line;
	for (const FString& Word : Words)
	{
		const FString Try = Line.IsEmpty() ? Word : Line + TEXT(" ") + Word;
		if (!Line.IsEmpty() && TextWidth(Try, Scale) > MaxWidth)
		{
			Lines.Add(Line);
			Line = Word;
		}
		else
		{
			Line = Try;
		}
	}
	if (!Line.IsEmpty())
	{
		Lines.Add(Line);
	}
	return Lines;
}

FString AGamerHUD::DescribeOwnCharacter() const
{
	const AGamerPlayerState* State = PlayerOwner ? PlayerOwner->GetPlayerState<AGamerPlayerState>() : nullptr;
	return State && State->HasCharacter() ? TEXT("Charakter: ") + State->Describe() : FString();
}

void AGamerHUD::DrawCharacterMenu(const AGamerPlayerController& PC)
{
	const float Width = Canvas->ClipX;
	const float Height = Canvas->ClipY;
	DrawRect(FLinearColor(0.06f, 0.07f, 0.08f, 1.f), 0.f, 0.f, Width, Height);

	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const FGamerCharacterProfile& Profile = UGamerCharacterSettings::Get()->Profile;
	const FGamerCharacterDataSet* Characters = Data ? &Data->GetCharacters() : nullptr;
	const FGamerOriginDef* Origin = Characters ? Characters->FindOrigin(Profile.Origin) : nullptr;
	const FString NextText = PC.GetCharacterNext() == EGamerMenuPage::Join ? TEXT("Weiter: Beitreten") : TEXT("Weiter: Welt erstellen");
	const FString BackText = TEXT("Zurück");

	if (!Characters || !Origin)
	{
		const float S = Ui;
		DrawCentered(TEXT("Charakterdaten fehlen (Content/Data/characters.json)."), Height * 0.3f, CharHint, 1.f);
		const float ButtonW = TextWidth(NextText, S) + 60.f * S;
		DrawButton(TEXT("char:next"), NextText, (Width - ButtonW) * 0.5f, Height * 0.45f, ButtonW, LineHeight(S) * 1.5f, S, true);
		return;
	}

	if (PC.GetMenuPage() == EGamerMenuPage::CharacterLook)
	{
		// Appearance editor: one row per slot, click the value for the next option.
		float S = Ui;
		const int32 Rows = Characters->LookSlots.Num();
		S *= FMath::Min(1.f, Height * 0.9f / (LineHeight(Ui) * (Rows * 1.25f + 7.f)));
		const float RowH = LineHeight(S) * 1.15f;
		float LabelW = 0.f, ValueW = 0.f;
		for (const FGamerLookSlotDef& Slot : Characters->LookSlots)
		{
			LabelW = FMath::Max(LabelW, TextWidth(Slot.Name, S));
			for (const FGamerLookOption& Option : Slot.Options)
			{
				ValueW = FMath::Max(ValueW, TextWidth(Option.Name, S));
			}
		}
		ValueW += 50.f * S;
		const float X = (Width - (LabelW + 20.f * S + ValueW)) * 0.5f;
		float Y = Height * 0.05f;
		const FString Title = FString::Printf(TEXT("Aussehen: %s"), *Profile.Name);
		DrawShadowText(Title, (Width - TextWidth(Title, S * 1.3f)) * 0.5f, Y, CharWhite, S * 1.3f);
		Y += LineHeight(S * 1.3f) * 1.2f;
		for (const FString& Line : WrapText(TEXT("Wert anklicken für die nächste Möglichkeit. Das Figurenmodell (MetaHuman) fehlt noch: "
			"die Auswahl wird gespeichert und später auf die Figur übertragen. Werte und Ausrüstung bleiben die der Herkunft."), Width * 0.8f, S * 0.65f))
		{
			DrawShadowText(Line, (Width - TextWidth(Line, S * 0.65f)) * 0.5f, Y, CharDim, S * 0.65f);
			Y += LineHeight(S * 0.65f);
		}
		Y += LineHeight(S * 0.65f) * 0.6f;
		for (const FGamerLookSlotDef& Slot : Characters->LookSlots)
		{
			const FName Current = Profile.GetLook(Slot.Id);
			const FGamerLookOption* Option = Slot.Options.FindByPredicate([Current](const FGamerLookOption& O) { return O.Id == Current; });
			DrawShadowText(Slot.Name, X, Y + (RowH - LineHeight(S) / 1.12f) * 0.5f, CharWhite, S);
			DrawButton(FName(*(TEXT("char:look:") + Slot.Id.ToString())), Option ? Option->Name : TEXT("?"), X + LabelW + 20.f * S, Y, ValueW, RowH, S);
			Y += RowH + 4.f * S;
		}
		Y += 8.f * S;
		const FString ResetText = TEXT("Vorschlag der Herkunft");
		const FString DoneText = TEXT("Fertig");
		const float ButtonH = LineHeight(S) * 1.4f;
		const float ResetW = TextWidth(ResetText, S) + 40.f * S;
		const float DoneW = TextWidth(DoneText, S) + 60.f * S;
		const float ButtonsX = (Width - (DoneW + 20.f * S + ResetW)) * 0.5f;
		DrawButton(TEXT("char:lookdone"), DoneText, ButtonsX, Y, DoneW, ButtonH, S, true);
		DrawButton(TEXT("char:lookreset"), ResetText, ButtonsX + DoneW + 20.f * S, Y, ResetW, ButtonH, S);
		Y += ButtonH + 8.f * S;
		if (!PC.GetMenuNote().IsEmpty())
		{
			DrawShadowText(PC.GetMenuNote(), (Width - TextWidth(PC.GetMenuNote(), S * 0.75f)) * 0.5f, Y, CharHint, S * 0.75f);
		}
		return;
	}

	// Character page: origin buttons on top, two columns (name, values | equipment, abilities, look), buttons below.
	auto Layout = [&](float S, bool bDraw) -> float
	{
		const float Margin = Width * 0.04f;
		const float FullW = Width - 2.f * Margin;
		const float ColGap = 40.f * S;
		const float ColW = (FullW - ColGap) * 0.5f;
		const float Body = S * 0.75f;
		const float Small = S * 0.62f;
		float Y = Height * 0.03f;

		auto Text = [&](const FString& Line, float X, float& InOutY, const FLinearColor& Color, float Scale)
		{
			if (bDraw)
			{
				DrawShadowText(Line, X, InOutY, Color, Scale);
			}
			InOutY += LineHeight(Scale);
		};
		auto Wrapped = [&](const FString& Para, float X, float W, float& InOutY, const FLinearColor& Color, float Scale)
		{
			for (const FString& Line : WrapText(Para, W, Scale))
			{
				Text(Line, X, InOutY, Color, Scale);
			}
		};

		const FString Title = TEXT("Charakter");
		Text(Title, (Width - TextWidth(Title, S * 1.3f)) * 0.5f, Y, CharWhite, S * 1.3f);
		const FString Sub = TEXT("Herkunft wählen und Namen tippen. Die Herkunft gibt Startwerte, Startausrüstung und eine Startfähigkeit; lernen kann später jeder alles.");
		for (const FString& Line : WrapText(Sub, FullW, Small))
		{
			Text(Line, (Width - TextWidth(Line, Small)) * 0.5f, Y, CharDim, Small);
		}
		Y += 6.f * S;

		// Origin row.
		const int32 Count = Characters->Origins.Num();
		const float OriginGap = 10.f * S;
		const float OriginW = (FullW - OriginGap * (Count - 1)) / FMath::Max(1, Count);
		float OriginScale = Body;
		for (const FGamerOriginDef& Def : Characters->Origins)
		{
			OriginScale = FMath::Min(OriginScale, Body * (OriginW - 16.f * S) / FMath::Max(1.f, TextWidth(Def.Name, Body)));
		}
		const float OriginH = LineHeight(Body) * 1.5f;
		if (bDraw)
		{
			for (int32 Index = 0; Index < Count; ++Index)
			{
				const FGamerOriginDef& Def = Characters->Origins[Index];
				DrawButton(FName(*(TEXT("char:origin:") + Def.Id.ToString())), Def.Name, Margin + Index * (OriginW + OriginGap), Y, OriginW, OriginH,
					OriginScale, Def.Id == Origin->Id);
			}
		}
		Y += OriginH + 12.f * S;
		const float ColumnsTop = Y;

		// Left column: name, role, description, values.
		const float LX = Margin;
		float LY = ColumnsTop;
		Text(TEXT("Name"), LX, LY, CharHint, Body);
		const float FieldH = LineHeight(Body) * 1.35f;
		const FString RandomText = TEXT("Zufallsname");
		const float RandomW = TextWidth(RandomText, Body) + 30.f * S;
		const float FieldW = FMath::Min(ColW - RandomW - 12.f * S, TextWidth(TEXT("WWWWWWWWWWWWWWWW"), Body) + 30.f * S);
		if (bDraw)
		{
			DrawRect(CharHint, LX - 2.f, LY - 2.f, FieldW + 4.f, FieldH + 4.f);
			DrawRect(FLinearColor(0.02f, 0.02f, 0.02f, 1.f), LX, LY, FieldW, FieldH);
			const bool bCaret = FMath::Fmod(GetWorld()->GetRealTimeSeconds(), 1.0) < 0.6;
			DrawShadowText(Profile.Name + (bCaret ? TEXT("|") : TEXT(" ")), LX + 10.f * S, LY + (FieldH - LineHeight(Body) / 1.12f) * 0.5f, CharWhite, Body);
			DrawButton(TEXT("char:randomname"), RandomText, LX + FieldW + 12.f * S, LY, RandomW, FieldH, Body);
		}
		LY += FieldH + 4.f * S;
		Text(TEXT("Tippen, Rücktaste löscht, Eingabe = Weiter."), LX, LY, CharDim, Small);
		LY += 6.f * S;
		Text(TEXT("Stärke im Koop: ") + Origin->Role, LX, LY, CharGood, Body);
		Wrapped(Origin->Description, LX, ColW, LY, CharDim, Small);
		LY += 6.f * S;
		Text(FString::Printf(TEXT("Werte (Summe %d, je %d–%d)"), Characters->StatSum, Characters->StatMin, Characters->StatMax), LX, LY, CharHint, Body);
		float StatNameW = 0.f;
		for (const FGamerStatDef& Stat : Characters->Stats)
		{
			StatNameW = FMath::Max(StatNameW, TextWidth(Stat.Name, Body));
		}
		for (const FGamerStatDef& Stat : Characters->Stats)
		{
			const int32 Value = Origin->GetStat(Stat.Id);
			const float RowH = LineHeight(Body);
			if (bDraw)
			{
				DrawShadowText(Stat.Name, LX, LY, CharWhite, Body);
				const float BarX = LX + StatNameW + 14.f * S;
				const float BarW = FMath::Max(40.f, ColW * 0.22f);
				const float BarH = RowH * 0.45f;
				const float BarY = LY + (RowH - BarH) * 0.45f;
				DrawRect(FLinearColor(0.15f, 0.15f, 0.15f, 1.f), BarX, BarY, BarW, BarH);
				DrawRect(FLinearColor(0.85f, 0.65f, 0.20f, 1.f), BarX, BarY, BarW * Value / FMath::Max(1, Characters->StatMax), BarH);
				DrawShadowText(FString::FromInt(Value), BarX + BarW + 10.f * S, LY, CharWhite, Body);
				DrawShadowText(Stat.Affects, BarX + BarW + 10.f * S + TextWidth(TEXT("00"), Body) + 10.f * S, LY + (RowH - LineHeight(Small)) * 0.5f, CharDim,
					FMath::Min(Small, Small * (ColW - (BarX - LX) - BarW - TextWidth(TEXT("00"), Body) - 20.f * S) / FMath::Max(1.f, TextWidth(Stat.Affects, Small))));
			}
			LY += RowH;
		}
		Text(TEXT("Wirkung der Werte folgt mit Kampf, Handwerk und Magie."), LX, LY, CharDim, Small);

		// Right column: start equipment, start ability, own branch, look.
		const float RX = Margin + ColW + ColGap;
		float RY = ColumnsTop;
		Text(TEXT("Startausrüstung"), RX, RY, CharHint, Body);
		for (const FGamerItemAmount& Amount : Origin->StartItems)
		{
			const FGamerItemDef* Item = Data->FindItem(Amount.Item);
			Text(FString::Printf(TEXT("%d × %s"), Amount.Count, Item ? *Item->Name : *Amount.Item.ToString()), RX + 10.f * S, RY, CharWhite, Body);
		}
		RY += 6.f * S;
		Text(TEXT("Startfähigkeit"), RX, RY, CharHint, Body);
		if (const FGamerAbilityDef* Ability = Characters->FindAbility(Origin->StartAbility))
		{
			Text(Ability->Name, RX + 10.f * S, RY, CharWhite, Body);
			Wrapped(Ability->Description, RX + 10.f * S, ColW - 10.f * S, RY, CharDim, Small);
		}
		RY += 6.f * S;
		Text(TEXT("Eigener Zweig (später günstiger lernbar)"), RX, RY, CharHint, Body);
		FString Branch;
		for (const FGamerAbilityDef& Ability : Characters->Abilities)
		{
			if (Ability.Branch == Origin->Id && Ability.Id != Origin->StartAbility)
			{
				Branch += (Branch.IsEmpty() ? TEXT("") : TEXT(", ")) + Ability.Name;
			}
		}
		Wrapped(Branch, RX + 10.f * S, ColW - 10.f * S, RY, CharDim, Small);
		RY += 6.f * S;
		Text(TEXT("Aussehen"), RX, RY, CharHint, Body);
		FString Look;
		for (const FGamerLookSlotDef& Slot : Characters->LookSlots)
		{
			const FName Current = Profile.GetLook(Slot.Id);
			const FGamerLookOption* Option = Slot.Options.FindByPredicate([Current](const FGamerLookOption& O) { return O.Id == Current; });
			Look += FString::Printf(TEXT("%s%s: %s"), Look.IsEmpty() ? TEXT("") : TEXT(" · "), *Slot.Name, Option ? *Option->Name : TEXT("?"));
		}
		Wrapped(Look, RX + 10.f * S, ColW - 10.f * S, RY, CharDim, Small);
		RY += 4.f * S;
		const FString EditText = TEXT("Aussehen bearbeiten");
		const float EditH = LineHeight(Body) * 1.35f;
		if (bDraw)
		{
			DrawButton(TEXT("char:edit"), EditText, RX + 10.f * S, RY, TextWidth(EditText, Body) + 40.f * S, EditH, Body);
		}
		RY += EditH;

		// Bottom: note and buttons.
		Y = FMath::Max(LY, RY) + 12.f * S;
		if (!PC.GetMenuNote().IsEmpty())
		{
			Text(PC.GetMenuNote(), (Width - TextWidth(PC.GetMenuNote(), Body)) * 0.5f, Y, CharHint, Body);
		}
		const float ButtonH = LineHeight(S) * 1.4f;
		const float NextW = TextWidth(NextText, S) + 60.f * S;
		const float BackW = TextWidth(BackText, S) + 60.f * S;
		if (bDraw)
		{
			const float ButtonsX = (Width - (NextW + 20.f * S + BackW)) * 0.5f;
			DrawButton(TEXT("char:next"), NextText, ButtonsX, Y, NextW, ButtonH, S, true);
			DrawButton(TEXT("back"), BackText, ButtonsX + NextW + 20.f * S, Y, BackW, ButtonH, S);
		}
		return Y + ButtonH + Height * 0.02f;
	};

	// Shrink until everything fits the window (small windows, big text setting).
	float S = Ui;
	for (int32 Pass = 0; Pass < 4; ++Pass)
	{
		const float Needed = Layout(S, false);
		if (Needed <= Height)
		{
			break;
		}
		S *= FMath::Max(0.5f, Height / Needed * 0.98f);
	}
	Layout(S, true);
}

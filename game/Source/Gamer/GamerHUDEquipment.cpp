// HUD part of equipment and level (docs/25): a thin level bar above the hit points, the equipment window (key C), and short
// messages (level reached, "inventory full"). Own file so GamerHUD.cpp stays small. Placeholder look like the rest of the HUD.
#include "GamerHUD.h"

#include "GamerCharacter.h"
#include "GamerDataSubsystem.h"
#include "GamerEquipmentComponent.h"
#include "GamerHealthComponent.h"
#include "GamerKeyBindings.h"
#include "GamerProgressComponent.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"

namespace
{
	FString SlotLabel(FName Slot)
	{
		static const TMap<FName, FString> Labels = { { TEXT("helmet"), TEXT("Helm") }, { TEXT("shoulder"), TEXT("Schultern") },
			{ TEXT("chest"), TEXT("Oberkörper") }, { TEXT("hands"), TEXT("Hände") }, { TEXT("legs"), TEXT("Beine") }, { TEXT("feet"), TEXT("Füße") },
			{ TEXT("cloak"), TEXT("Umhang") }, { TEXT("ring"), TEXT("Ringe") } };
		const FString* Found = Labels.Find(Slot);
		return Found ? *Found : Slot.ToString();
	}

	FString PieceText(const FGamerItemDef* Def)
	{
		if (!Def)
		{
			return TEXT("–");
		}
		FString Values;
		if (Def->Armor.Armor > 0.f) { Values += FString::Printf(TEXT("Rüstung %.0f"), Def->Armor.Armor); }
		if (Def->Armor.PoiseResist > 0.f) { Values += FString::Printf(TEXT("%sHaltung %.0f"), Values.IsEmpty() ? TEXT("") : TEXT(", "), Def->Armor.PoiseResist); }
		for (const TPair<FName, float>& Pair : Def->Armor.Resist)
		{
			static const TMap<FName, FString> Kinds = { { TEXT("heat"), TEXT("Hitze") }, { TEXT("cold"), TEXT("Kälte") },
				{ TEXT("storm"), TEXT("Sturm") }, { TEXT("poison"), TEXT("Gift") } };
			Values += FString::Printf(TEXT("%s%s %.0f %%"), Values.IsEmpty() ? TEXT("") : TEXT(", "), *Kinds.FindRef(Pair.Key), Pair.Value * 100.f);
		}
		return Values.IsEmpty() ? Def->Name : FString::Printf(TEXT("%s (%s)"), *Def->Name, *Values);
	}
}

void AGamerHUD::DrawEquipment(const AGamerCharacter* Character)
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const UGamerEquipmentComponent* Gear = Character ? Character->GetEquipment() : nullptr;
	const UGamerProgressComponent* Progress = Character ? Character->GetProgress() : nullptr;
	const UGamerHealthComponent* Health = Character ? Character->GetHealth() : nullptr;
	if (!Data || !Gear || !Progress || !Canvas || (Health && Health->IsIncapacitated()))
	{
		return;
	}
	const float Width = Canvas->ClipX;
	const float Height = Canvas->ClipY;
	const float Margin = 10.f * Ui;
	const double Now = GetWorld()->GetTimeSeconds();
	const FLinearColor EquipTextWhite(0.97f, 0.97f, 0.94f);
	const FLinearColor EquipTextDim(0.82f, 0.82f, 0.80f);
	const FLinearColor EquipTextGood(0.65f, 0.80f, 0.56f);

	// --- Level strip above the hit points (bottom left) ---
	{
		const float BarY = Height - 80.f * Ui;
		const float StripH = 5.f * Ui;
		const float StripW = 200.f * Ui;
		const int32 Level = Progress->GetLevel();
		const bool bMax = Progress->GetXpForNextLevel() <= 0;
		const FString Text = bMax ? FString::Printf(TEXT("Level %d (Höchststufe)"), Level)
			: FString::Printf(TEXT("Level %d   %d / %d EP"), Level, Progress->GetXpIntoLevel(), Progress->GetXpForNextLevel());
		// Above the stamina block (S bar and the hunger note sit between this strip and the hit points).
		const float TextY = BarY - 98.f * Ui - LineHeight(Ui * 0.75f) - StripH;
		DrawShadowText(Text, Margin, TextY, EquipTextDim, Ui * 0.75f);
		const float Fraction = bMax ? 1.f : FMath::Clamp(static_cast<float>(Progress->GetXpIntoLevel()) / static_cast<float>(FMath::Max(1, Progress->GetXpForNextLevel())), 0.f, 1.f);
		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.7f), Margin - 1.f, TextY + LineHeight(Ui * 0.75f) + 1.f, StripW + 2.f, StripH + 2.f);
		DrawRect(FLinearColor(0.22f, 0.22f, 0.22f, 0.9f), Margin, TextY + LineHeight(Ui * 0.75f) + 2.f, StripW, StripH);
		DrawRect(FLinearColor(0.55f, 0.43f, 0.15f, 0.95f), Margin, TextY + LineHeight(Ui * 0.75f) + 2.f, StripW * Fraction, StripH);
	}

	// --- Messages: level reached, server notices ---
	{
		float Y = Height * 0.18f;
		if (Now - Progress->GetLastLevelUpTime() < 5.0 && Progress->GetLastLevelUp() > 0)
		{
			const FString Text = FString::Printf(TEXT("Level %d erreicht"), Progress->GetLastLevelUp());
			DrawShadowText(Text, (Width - TextWidth(Text, Ui * 1.6f)) * 0.5f, Y, EquipTextGood, Ui * 1.6f);
			Y += LineHeight(Ui * 1.6f) * 1.1f;
		}
		if (Now - Gear->GetLastNoticeTime() < 4.0 && !Gear->GetLastNotice().IsEmpty())
		{
			DrawShadowText(Gear->GetLastNotice(), (Width - TextWidth(Gear->GetLastNotice(), Ui)) * 0.5f, Y, EquipTextWhite, Ui);
		}
	}

	if (!Character->IsGearMenuOpen())
	{
		return;
	}

	// --- Equipment window (left, below the middle of the screen's top) ---
	const FGamerCombatData& Combat = Data->GetCombat();
	const FGamerEquipmentTotals& Totals = Gear->GetTotals();
	TArray<FGamerHudLine> Lines;
	Lines.Add({ FString::Printf(TEXT("Ausrüstung   (%s schließt)   Level %d, Stufe %d"), *UGamerKeyBindings::ActionKeyName(GamerInputActions::GearMenu),
		Progress->GetLevel(), Progress->GetTier()), EquipTextWhite, 1.05f });
	for (int32 Row = 0; Row < 8; ++Row)
	{
		const FString Key = UGamerKeyBindings::ActionKeyName(GamerInputActions::Hotbar(Row + 1));
		if (Row < 7)
		{
			const FGamerItemDef* Worn = Gear->GetWornDef(Row);
			const FName Candidate = Worn ? NAME_None : Gear->FindCandidateFor(Row);
			const FGamerItemDef* Ready = Candidate.IsNone() ? nullptr : Data->FindItem(Candidate);
			FString Text = FString::Printf(TEXT("%s   %s: %s"), *Key, *SlotLabel(UGamerEquipmentComponent::SlotOfPlace(Row)), *PieceText(Worn));
			if (!Worn && Ready)
			{
				Text += FString::Printf(TEXT("   [bereit: %s]"), *Ready->Name);
			}
			Lines.Add({ Text, Worn ? EquipTextWhite : (Ready ? EquipTextGood : EquipTextDim), 0.95f });
		}
		else
		{
			const FGamerItemDef* A = Gear->GetWornDef(7);
			const FGamerItemDef* B = Gear->GetWornDef(8);
			const FName Candidate = Gear->FindCandidateFor(7);
			FString Text = FString::Printf(TEXT("%s   Ringe: %s | %s"), *Key, *PieceText(A), *PieceText(B));
			const bool bFree = !A || !B;
			if (bFree && !Candidate.IsNone())
			{
				if (const FGamerItemDef* Ready = Data->FindItem(Candidate)) { Text += FString::Printf(TEXT("   [bereit: %s]"), *Ready->Name); }
			}
			Lines.Add({ Text, (A || B) ? EquipTextWhite : (bFree && !Candidate.IsNone() ? EquipTextGood : EquipTextDim), 0.95f });
		}
	}
	Lines.Add({ FString::Printf(TEXT("Rüstung: Kopf %.0f · Körper %.0f · Gliedmaßen %.0f   Haltung %.0f"),
		GamerEquipment::ZoneArmor(Totals, EGamerHitZone::Head, Combat), GamerEquipment::ZoneArmor(Totals, EGamerHitZone::Body, Combat),
		GamerEquipment::ZoneArmor(Totals, EGamerHitZone::Limbs, Combat), Totals.PoiseResist), EquipTextDim, 0.9f });
	Lines.Add({ FString::Printf(TEXT("Resistenz: Kälte %.0f %% · Gift %.0f %% · Hitze %.0f %% · Sturm %.0f %%   Gewicht %.1f kg"),
		Totals.GetResist(TEXT("cold")) * 100.f, Totals.GetResist(TEXT("poison")) * 100.f, Totals.GetResist(TEXT("heat")) * 100.f,
		Totals.GetResist(TEXT("storm")) * 100.f, Totals.WeightKg), EquipTextDim, 0.9f });
	Lines.Add({ FString::Printf(TEXT("Fähigkeitspunkte bisher: %d (der Fähigkeitenbaum fehlt noch)"), Progress->GetSkillPointsEarned()), EquipTextDim, 0.8f });
	// Top right (the crafting menu is closed while this window is open); the inventory list stays visible on the left.
	float BlockWidth = 0.f;
	for (const FGamerHudLine& Line : Lines)
	{
		BlockWidth = FMath::Max(BlockWidth, TextWidth(Line.Text, Ui * Line.Size));
	}
	DrawLineBlock(Lines, FMath::Max(Margin, Width - Margin - BlockWidth - 12.f * Ui), Margin);
}

#include "GamerPlayerState.h"

#include "Gamer.h"
#include "GamerCharacter.h"
#include "GamerDataSubsystem.h"
#include "GamerInventoryComponent.h"
#include "GamerSaveSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

AGamerPlayerState::AGamerPlayerState()
{
}

void AGamerPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGamerPlayerState, Profile);
	DOREPLIFETIME(AGamerPlayerState, Stats);
	DOREPLIFETIME(AGamerPlayerState, Abilities);
	DOREPLIFETIME(AGamerPlayerState, bHasCharacter);
}

void AGamerPlayerState::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		OnPawnSet.AddDynamic(this, &AGamerPlayerState::HandlePawnSet);
	}
	// The owner (our controller) may arrive a moment after the player state on a client: check a few times.
	TrySendLocalProfile();
	if (!bProfileSent && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(SendTimer, this, &AGamerPlayerState::TrySendLocalProfile, 0.25f, true);
	}
}

void AGamerPlayerState::TrySendLocalProfile()
{
	const APlayerController* PC = GetPlayerController();
	if (bProfileSent || IsABot())
	{
		GetWorldTimerManager().ClearTimer(SendTimer);
		return;
	}
	if (PC && !PC->IsLocalController())
	{
		GetWorldTimerManager().ClearTimer(SendTimer); // a remote player's state on the server: that client sends itself
		return;
	}
	if (!PC)
	{
		// Another player's state on this client (never gets an owner here), or ours before the owner replicated.
		if (GetNetMode() == NM_DedicatedServer || (GetWorld() && GetWorld()->GetTimeSeconds() - CreationTime > 30.f))
		{
			GetWorldTimerManager().ClearTimer(SendTimer);
		}
		return;
	}
	bProfileSent = true;
	GetWorldTimerManager().ClearTimer(SendTimer);

	FGamerCharacterProfile Local = UGamerCharacterSettings::Get()->Profile;
	// Tests (tools/mp_test.ps1): -GamerCharacter=origin_warrior:Ragnar plays that origin without the menu.
	FString Override;
	if (FParse::Value(FCommandLine::Get(), TEXT("GamerCharacter="), Override, false))
	{
		FString Origin, Name;
		if (!Override.Split(TEXT(":"), &Origin, &Name))
		{
			Origin = Override;
		}
		const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
		Local = Data ? Data->GetCharacters().MakeProfile(FName(*Origin), Name) : Local;
	}
	UE_LOG(LogGamer, Log, TEXT("Character: sending %s"), *Local.ToLogString());
	ServerSetCharacter(Local);
}

void AGamerPlayerState::ServerSetCharacter_Implementation(const FGamerCharacterProfile& InProfile)
{
	if (bHasCharacter)
	{
		UE_LOG(LogGamer, Warning, TEXT("Character: %s already has a character, request ignored"), *GetPlayerName());
		return;
	}
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (!Data || Data->GetCharacters().IsEmpty())
	{
		UE_LOG(LogGamer, Error, TEXT("Character: no character data on the server"));
		return;
	}
	const FGamerCharacterDataSet& Characters = Data->GetCharacters();
	FGamerCharacterProfile Checked = InProfile;
	if (!Characters.Sanitize(Checked))
	{
		UE_LOG(LogGamer, Warning, TEXT("Character: request corrected from %s"), *InProfile.ToLogString());
	}
	const FGamerOriginDef* Origin = Characters.FindOrigin(Checked.Origin);
	check(Origin); // Sanitize guarantees a known origin

	// Values and the start ability come from the server's data, never from the client.
	Profile = Checked;
	Stats = Origin->Stats;
	Abilities.Reset();
	if (!Origin->StartAbility.IsNone())
	{
		Abilities.Add(Origin->StartAbility);
	}
	bHasCharacter = true;
	SetPlayerName(Profile.Name);
	UE_LOG(LogGamer, Log, TEXT("Character applied: %s"), *Describe());
	ForceNetUpdate();
	GiveStartItems();
}

void AGamerPlayerState::HandlePawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	if (NewPawn)
	{
		GiveStartItems();
	}
}

void AGamerPlayerState::GiveStartItems()
{
	if (!HasAuthority() || bStartItemsGiven || !bHasCharacter)
	{
		return;
	}
	const AGamerCharacter* GamerCharacter = Cast<AGamerCharacter>(GetPawn());
	UGamerInventoryComponent* Inventory = GamerCharacter ? GamerCharacter->GetInventory() : nullptr;
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const FGamerOriginDef* Origin = Data ? Data->GetCharacters().FindOrigin(Profile.Origin) : nullptr;
	if (!Inventory || !Origin)
	{
		return; // again when the pawn is set
	}
	if (!Inventory->HasBegunPlay())
	{
		// Listen host / solo: the request comes before the new character's inventory has made its slots.
		GetWorldTimerManager().SetTimerForNextTick(this, &AGamerPlayerState::GiveStartItems);
		return;
	}
	// Save system (agreed with the character session 2026-11-15): wait until the player is matched with the world save;
	// a player this world already knows keeps the saved inventory and gets no second start equipment.
	if (const UGamerSaveSubsystem* Saves = UGamerSaveSubsystem::Get(this))
	{
		const AController* Controller = GetOwningController();
		if (!Saves->IsPlayerResolved(Controller))
		{
			FTimerHandle Retry;
			GetWorldTimerManager().SetTimer(Retry, this, &AGamerPlayerState::GiveStartItems, 0.25f, false);
			return;
		}
		if (Saves->HasSavedInventory(Controller))
		{
			bStartItemsGiven = true;
			UE_LOG(LogGamer, Log, TEXT("Character start items for %s: none, inventory comes from the world save"), *GetPlayerName());
			return;
		}
	}
	bStartItemsGiven = true;
	for (const FGamerItemAmount& Amount : Origin->StartItems)
	{
		const int32 Left = Inventory->AddItem(Amount.Item, Amount.Count);
		if (Left > 0)
		{
			UE_LOG(LogGamer, Warning, TEXT("Character: %d x %s did not fit into the inventory"), Left, *Amount.Item.ToString());
		}
	}
	UE_LOG(LogGamer, Log, TEXT("Character start items for %s: %s"), *GetPlayerName(), *Inventory->Describe());
}

int32 AGamerPlayerState::GetStat(FName Stat) const
{
	const FGamerStatValue* Found = Stats.FindByPredicate([Stat](const FGamerStatValue& V) { return V.Stat == Stat; });
	return Found ? Found->Value : 0;
}

FString AGamerPlayerState::Describe() const
{
	if (!bHasCharacter)
	{
		return GetPlayerName() + TEXT(" (kein Charakter)");
	}
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const FGamerCharacterDataSet* Characters = Data ? &Data->GetCharacters() : nullptr;
	const FGamerOriginDef* Origin = Characters ? Characters->FindOrigin(Profile.Origin) : nullptr;
	FString Text = FString::Printf(TEXT("%s, %s ·"), *Profile.Name, Origin ? *Origin->Name : *Profile.Origin.ToString());
	for (const FGamerStatValue& Value : Stats)
	{
		const FGamerStatDef* Stat = Characters ? Characters->FindStat(Value.Stat) : nullptr;
		Text += FString::Printf(TEXT(" %s %d"), Stat ? *Stat->Short : *Value.Stat.ToString(), Value.Value);
	}
	for (const FName Ability : Abilities)
	{
		const FGamerAbilityDef* Def = Characters ? Characters->FindAbility(Ability) : nullptr;
		Text += FString::Printf(TEXT(" · %s"), Def ? *Def->Name : *Ability.ToString());
	}
	return Text;
}

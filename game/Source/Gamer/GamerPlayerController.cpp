#include "GamerPlayerController.h"

#include "Gamer.h"
#include "GamerBuildComponent.h"
#include "GamerBuilding.h"
#include "GamerCharacter.h"
#include "GamerDataTypes.h"
#include "GamerGraphicsSettings.h"
#include "GamerHUD.h"
#include "GamerInventoryComponent.h"
#include "GamerKeyBindings.h"
#include "GamerMenuGameMode.h"
#include "GamerSessionSubsystem.h"
#include "GamerResourceNode.h"
#include "GamerSettlement.h"
#include "GamerVillager.h"
#include "GamerDataSubsystem.h"
#include "GamerWorkComponent.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "EngineUtils.h"
#include "Engine/GameViewportClient.h"
#include "Kismet/GameplayStatics.h"
#include "InputKeyEventArgs.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "UnrealClient.h"

bool AGamerPlayerController::InputKey(const FInputKeyEventArgs& Params)
{
	const bool bPressed = Params.Event == IE_Pressed;
	const bool bMenuKey = Params.Key == EKeys::Escape || Params.Key == EKeys::Gamepad_Special_Right;

	if (IsMenuOpen() && bPressed && !Params.Key.IsAnalog())
	{
		// Controls page: the next key becomes the new binding.
		if (!ListeningAction.IsNone())
		{
			const FName Action = ListeningAction;
			if (bMenuKey)
			{
				ListeningAction = NAME_None;
				MenuNote = TEXT("Abgebrochen.");
			}
			else if (!UGamerKeyBindings::CanBind(Params.Key))
			{
				MenuNote = TEXT("Diese Taste geht nicht. Controller-Tasten sind fest belegt.");
			}
			else
			{
				ListeningAction = NAME_None;
				UGamerKeyBindings* Keys = UGamerKeyBindings::Get();
				const FName Swapped = Keys->SetKey(Action, Params.Key);
				MenuNote = FString::Printf(TEXT("%s: %s"), *UGamerKeyBindings::ActionLabel(Action), *UGamerKeyBindings::KeyName(Params.Key));
				if (!Swapped.IsNone())
				{
					MenuNote += FString::Printf(TEXT("   (getauscht: %s jetzt %s)"),
						*UGamerKeyBindings::ActionLabel(Swapped), *UGamerKeyBindings::ActionKeyName(Swapped));
				}
				ApplyKeyBindingsToPawn();
			}
			return true;
		}
		if (MenuPage == EGamerMenuPage::Join && TypeIntoAddress(Params.Key))
		{
			return true;
		}
		if (bMenuKey)
		{
			if (MenuPage == EGamerMenuPage::Controls || MenuPage == EGamerMenuPage::CreateWorld || MenuPage == EGamerMenuPage::Join
				|| MenuPage == EGamerMenuPage::Graphics)
			{
				PressMenuButton(TEXT("back"));
			}
			else if (!bInStartMenu)
			{
				CloseMenu();
			}
			return true;
		}
		if (Params.Key == EKeys::LeftMouseButton)
		{
			float X = 0.f, Y = 0.f;
			const AGamerHUD* GamerHUD = Cast<AGamerHUD>(GetHUD());
			if (GamerHUD && GetMousePosition(X, Y))
			{
				const FName Button = GamerHUD->FindMenuButtonAt(FVector2D(X, Y));
				if (!Button.IsNone())
				{
					PressMenuButton(Button);
				}
			}
			return true;
		}
		return true; // other presses do nothing while the menu is open (releases still pass below)
	}

	const FKey BirdKey = UGamerKeyBindings::Get()->GetKey(GamerInputActions::BirdView);
	if (bBirdView && !IsMenuOpen())
	{
		if (bPressed && (bMenuKey || Params.Key == BirdKey))
		{
			SetBirdView(false);
			return true;
		}
		if (bPressed && Params.Key == UGamerKeyBindings::Get()->GetKey(GamerInputActions::VillageMenu))
		{
			OpenMenu(EGamerMenuPage::Village); // closing it returns to the bird's-eye view
			return true;
		}
		if (bPressed && Params.Key == EKeys::LeftMouseButton)
		{
			float X = 0.f, Y = 0.f;
			if (GetMousePosition(X, Y))
			{
				const AGamerHUD* GamerHUD = Cast<AGamerHUD>(GetHUD());
				const FName Button = GamerHUD ? GamerHUD->FindMenuButtonAt(FVector2D(X, Y)) : NAME_None;
				if (!Button.IsNone())
				{
					PressMenuButton(Button);
				}
				else
				{
					SelectBirdTarget(FindBirdTargetAt(FVector2D(X, Y)));
				}
			}
			return true;
		}
		if (bPressed && (Params.Key == EKeys::MouseScrollUp || Params.Key == EKeys::MouseScrollDown))
		{
			BirdDistance = FMath::Clamp(BirdDistance * (Params.Key == EKeys::MouseScrollUp ? 0.87f : 1.15f), 800.f, 8000.f);
			return true;
		}
		return Super::InputKey(Params); // move keys and the right mouse button are read in UpdateBirdView
	}
	if (!IsMenuOpen() && bPressed && Params.Key == BirdKey && !bInStartMenu && GetPawn())
	{
		SetBirdView(true);
		return true;
	}

	if (!IsMenuOpen() && bPressed && bMenuKey)
	{
		OpenMenu();
		return true;
	}
	return Super::InputKey(Params);
}

void AGamerPlayerController::OpenMenu(EGamerMenuPage Page)
{
	if (!IsLocalController() || Page == EGamerMenuPage::None)
	{
		return;
	}
	const bool bWasOpen = IsMenuOpen();
	MenuPage = Page;
	ListeningAction = NAME_None;
	MenuNote.Reset();
	if (bWasOpen)
	{
		return;
	}
	// The game keeps running (co-op): only this player's input is switched to the menu.
	FlushPressedKeys();
	ApplyInputModeForState();
	UE_LOG(LogGamer, Log, TEXT("Menu opened"));
}

void AGamerPlayerController::ApplyInputModeForState()
{
	const bool bCursor = IsMenuOpen() || bBirdView;
	SetShowMouseCursor(bCursor);
	if (bCursor)
	{
		FInputModeGameAndUI Mode;
		Mode.SetHideCursorDuringCapture(false);
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(Mode);
	}
	else
	{
		SetInputMode(FInputModeGameOnly());
	}
	if (AGamerCharacter* GamerCharacter = Cast<AGamerCharacter>(GetPawn()))
	{
		GamerCharacter->SetGameplayInputEnabled(!bCursor);
	}
}

void AGamerPlayerController::CloseMenu()
{
	if (!IsMenuOpen() || bInStartMenu)
	{
		return; // the start menu cannot be closed, only left by starting or joining a world
	}
	MenuPage = EGamerMenuPage::None;
	ListeningAction = NAME_None;
	MenuNote.Reset();
	FlushPressedKeys();
	ApplyInputModeForState(); // back to the character, or to the bird's-eye view it was opened from
	UE_LOG(LogGamer, Log, TEXT("Menu closed"));
}

void AGamerPlayerController::PressMenuButton(FName ButtonId)
{
	const FString Id = ButtonId.ToString();
	if (Id == TEXT("resume"))
	{
		CloseMenu();
	}
	else if (Id == TEXT("controls"))
	{
		MenuPage = EGamerMenuPage::Controls;
		MenuNote.Reset();
	}
	else if (Id == TEXT("graphics"))
	{
		MenuPage = EGamerMenuPage::Graphics;
		MenuNote.Reset();
	}
	else if (Id == TEXT("gfx:auto"))
	{
		GamerGraphics::AutoDetect();
		MenuNote = TEXT("Automatisch eingestellt (kurzer Leistungstest der Engine).");
	}
	else if (Id == TEXT("gfx:defaults"))
	{
		GamerGraphics::ResetToDefaults();
		MenuNote = TEXT("Standard: Qualität Hoch, Render-Auflösung automatisch, V-Sync an. Die Anzeige bleibt.");
	}
	else if (Id.StartsWith(TEXT("gfx:")))
	{
		if (GamerGraphics::Cycle(ButtonId))
		{
			MenuNote.Reset();
		}
	}
	else if (Id == TEXT("back"))
	{
		MenuPage = bInStartMenu ? EGamerMenuPage::Start : EGamerMenuPage::Main;
		ListeningAction = NAME_None;
		MenuNote.Reset();
	}
	else if (Id == TEXT("leave"))
	{
		LeaveWorld();
	}
	else if (Id == TEXT("create"))
	{
		MenuPage = EGamerMenuPage::CreateWorld;
		MenuNote.Reset();
	}
	else if (Id == TEXT("joinpage"))
	{
		MenuPage = EGamerMenuPage::Join;
		JoinAddress = UGamerMenuSettings::Get()->JoinAddress;
		MenuNote.Reset();
	}
	else if (Id.StartsWith(TEXT("opt:")))
	{
		// Cycle one world option; remembered for the next start.
		FGamerWorldOptions& Options = UGamerMenuSettings::Get()->WorldOptions;
		const FString Which = Id.Mid(4);
		if (Which == TEXT("flow"))
		{
			Options.BuildFlow = static_cast<EGamerBuildFlow>((static_cast<uint8>(Options.BuildFlow) + 1) % 3);
		}
		else if (Which == TEXT("place"))
		{
			Options.Placement = static_cast<EGamerPlacement>((static_cast<uint8>(Options.Placement) + 1) % 2);
		}
		else if (Which == TEXT("demolish"))
		{
			Options.Demolish = static_cast<EGamerDemolish>((static_cast<uint8>(Options.Demolish) + 1) % 3);
		}
		UGamerMenuSettings::Get()->Save();
		UE_LOG(LogGamer, Log, TEXT("Start menu world options: %s"), *Options.ToLogString());
	}
	else if (Id == TEXT("start"))
	{
		HostWorld();
	}
	else if (Id.StartsWith(TEXT("take:")))
	{
		const FName Item(*Id.Mid(5));
		const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
		const FGamerItemDef* Def = Data ? Data->FindItem(Item) : nullptr;
		ServerStorageTake(Item, Def ? FMath::Max(1, Def->MaxStack) : 1);
	}
	else if (Id.StartsWith(TEXT("put:")))
	{
		ServerStorageDeposit(FName(*Id.Mid(4)), 9999);
	}
	else if (Id == TEXT("putall"))
	{
		ServerStorageDepositAll(false);
	}
	else if (Id.StartsWith(TEXT("job:")))
	{
		// Cycle the villager's job (the HUD knows which villager the button belongs to).
		const AGamerHUD* GamerHUD = Cast<AGamerHUD>(GetHUD());
		AGamerVillager* Villager = GamerHUD ? Cast<AGamerVillager>(GamerHUD->FindButtonTarget(ButtonId)) : nullptr;
		const TArray<FName> Order = GetJobOrder();
		if (Villager && Order.Num() > 0)
		{
			const int32 Current = Order.IndexOfByKey(Villager->GetJobId());
			ServerSetVillagerJob(Villager, Order[(Current + 1) % Order.Num()]);
		}
	}
	else if (Id == TEXT("birdjob"))
	{
		// Bird's-eye view: cycle the job of the selected villager.
		AGamerVillager* Villager = Cast<AGamerVillager>(BirdSelection.Get());
		const TArray<FName> Order = GetJobOrder();
		if (Villager && Order.Num() > 0)
		{
			const int32 Current = Order.IndexOfByKey(Villager->GetJobId());
			ServerSetVillagerJob(Villager, Order[(Current + 1) % Order.Num()]);
		}
	}
	else if (Id == TEXT("connect"))
	{
		JoinWorld();
	}
	else if (Id == TEXT("quit"))
	{
		ConsoleCommand(TEXT("quit"));
	}
	else if (Id == TEXT("reset"))
	{
		UGamerKeyBindings::Get()->ResetToDefaults();
		ApplyKeyBindingsToPawn();
		MenuNote = TEXT("Standardbelegung wiederhergestellt.");
	}
	else if (Id.StartsWith(TEXT("build:")))
	{
		if (AGamerCharacter* GamerCharacter = Cast<AGamerCharacter>(GetPawn()))
		{
			const FString Piece = Id.Mid(6);
			GamerCharacter->GetBuild()->SelectBuildable(Piece == TEXT("none") ? NAME_None : FName(*Piece));
		}
		CloseMenu();
	}
	else if (Id.StartsWith(TEXT("bind:")))
	{
		ListeningAction = FName(*Id.Mid(5));
		MenuNote = FString::Printf(TEXT("%s: neue Taste drücken (Esc bricht ab)"), *UGamerKeyBindings::ActionLabel(ListeningAction));
	}
}

void AGamerPlayerController::HostWorld()
{
	// This player becomes the host (listen server); others join by address. A dedicated server works the same
	// way with the options on its command line (tools/mp_test.ps1, GamerServer.exe).
	UGamerMenuSettings* Settings = UGamerMenuSettings::Get();
	Settings->Save();
	if (UGamerSessionSubsystem* Session = UGamerSessionSubsystem::Get(this))
	{
		Session->ClearError();
	}
	const FString Options = TEXT("listen?game=/Script/Gamer.GamerGameMode") + Settings->WorldOptions.ToUrlOptions();
	UE_LOG(LogGamer, Log, TEXT("Hosting world %s?%s"), GamerMaps::World, *Options);
	MenuNote = TEXT("Welt wird gestartet …");
	UGameplayStatics::OpenLevel(this, FName(GamerMaps::World), true, Options);
}

void AGamerPlayerController::JoinWorld()
{
	const FString Address = JoinAddress.TrimStartAndEnd();
	if (Address.IsEmpty())
	{
		MenuNote = TEXT("Bitte die Adresse des Gastgebers eingeben.");
		return;
	}
	UGamerMenuSettings* Settings = UGamerMenuSettings::Get();
	Settings->JoinAddress = Address;
	Settings->Save();
	if (UGamerSessionSubsystem* Session = UGamerSessionSubsystem::Get(this))
	{
		Session->ClearError();
	}
	UE_LOG(LogGamer, Log, TEXT("Joining %s"), *Address);
	MenuNote = FString::Printf(TEXT("Verbinde mit %s …"), *Address);
	ClientTravel(Address, TRAVEL_Absolute);
}

void AGamerPlayerController::LeaveWorld()
{
	// A client just disconnects; a listen host closes the world for everyone (they return to their start menu).
	UE_LOG(LogGamer, Log, TEXT("Leaving the world for the start menu"));
	UGameplayStatics::OpenLevel(this, FName(GamerMaps::StartMenu));
}

bool AGamerPlayerController::TypeIntoAddress(const FKey& Key)
{
	if (Key == EKeys::BackSpace)
	{
		JoinAddress.LeftChopInline(1);
		return true;
	}
	if (Key == EKeys::Enter)
	{
		JoinWorld();
		return true;
	}
	const bool bShift = IsInputKeyDown(EKeys::LeftShift) || IsInputKeyDown(EKeys::RightShift);
	TCHAR Char = 0;
	static const FKey Digits[] = { EKeys::Zero, EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four,
		EKeys::Five, EKeys::Six, EKeys::Seven, EKeys::Eight, EKeys::Nine };
	static const FKey PadDigits[] = { EKeys::NumPadZero, EKeys::NumPadOne, EKeys::NumPadTwo, EKeys::NumPadThree, EKeys::NumPadFour,
		EKeys::NumPadFive, EKeys::NumPadSix, EKeys::NumPadSeven, EKeys::NumPadEight, EKeys::NumPadNine };
	for (int32 Index = 0; Index < 10; ++Index)
	{
		if (Key == Digits[Index] || Key == PadDigits[Index])
		{
			Char = static_cast<TCHAR>(TEXT('0') + Index);
		}
	}
	if (Key == EKeys::Period || Key == EKeys::Decimal)
	{
		Char = bShift ? TEXT(':') : TEXT('.'); // German layout: Shift + . = :
	}
	else if (Key == EKeys::Semicolon && bShift)
	{
		Char = TEXT(':'); // US layout
	}
	else if (Key == EKeys::Hyphen || Key == EKeys::Subtract)
	{
		Char = TEXT('-');
	}
	else if (Char == 0)
	{
		const FString Name = Key.GetFName().ToString();
		if (Name.Len() == 1 && FChar::IsAlpha(Name[0]))
		{
			Char = FChar::ToLower(Name[0]);
		}
	}
	if (Char == 0)
	{
		return false;
	}
	if (JoinAddress.Len() < 64)
	{
		JoinAddress.AppendChar(Char);
	}
	return true;
}

TArray<AGamerVillager*> AGamerPlayerController::GetVillagersSorted() const
{
	TArray<AGamerVillager*> Villagers;
	for (TActorIterator<AGamerVillager> It(GetWorld()); It; ++It)
	{
		if (!It->IsActorBeingDestroyed())
		{
			Villagers.Add(*It);
		}
	}
	Villagers.Sort([](const AGamerVillager& A, const AGamerVillager& B) { return A.GetVillagerName() < B.GetVillagerName(); });
	return Villagers;
}

TArray<FName> AGamerPlayerController::GetJobOrder() const
{
	TArray<FName> Order;
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (!Data)
	{
		return Order;
	}
	TArray<const FGamerJobDef*> Jobs;
	for (const TPair<FName, FGamerJobDef>& Pair : Data->GetData().Jobs)
	{
		if (!Pair.Value.bDeprecated)
		{
			Jobs.Add(&Pair.Value);
		}
	}
	Jobs.Sort([](const FGamerJobDef& A, const FGamerJobDef& B) { return A.bFallback != B.bFallback ? A.bFallback : A.Name < B.Name; });
	for (const FGamerJobDef* Job : Jobs)
	{
		Order.Add(Job->Id);
	}
	return Order;
}

AGamerBuilding* AGamerPlayerController::StorageInReach() const
{
	const APawn* MyPawn = GetPawn();
	const AGamerSettlement* Settlement = AGamerSettlement::Get(this);
	AGamerBuilding* Storage = MyPawn && Settlement ? Settlement->FindNearestStorageBuilding(MyPawn->GetActorLocation()) : nullptr;
	return Storage && FVector::Dist(Storage->GetActorLocation(), MyPawn->GetActorLocation()) <= 700.f ? Storage : nullptr;
}

void AGamerPlayerController::ServerStorageTake_Implementation(FName Item, int32 Count)
{
	AGamerCharacter* GamerCharacter = Cast<AGamerCharacter>(GetPawn());
	AGamerSettlement* Settlement = AGamerSettlement::Get(this);
	if (!GamerCharacter || !Settlement || !StorageInReach())
	{
		return;
	}
	UGamerInventoryComponent* Inventory = GamerCharacter->GetInventory();
	const int32 Wanted = FMath::Min3(Count, Settlement->CountInStorage(Item), Inventory->SpaceFor(Item));
	if (Wanted <= 0)
	{
		GamerCharacter->GetWork()->ClientMessage(Inventory->SpaceFor(Item) <= 0 ? TEXT("Kein Platz im Inventar.") : TEXT("Davon ist nichts im Lager."));
		return;
	}
	const int32 Taken = Settlement->TakeFromStorage(Item, Wanted);
	const int32 Back = Inventory->AddItem(Item, Taken);
	Settlement->AddToStorage(Item, Back);
	UE_LOG(LogGamer, Log, TEXT("Storage take: %s x%d - %s"), *Item.ToString(), Taken - Back, *Settlement->Describe());
}

void AGamerPlayerController::ServerStorageDeposit_Implementation(FName Item, int32 Count)
{
	AGamerCharacter* GamerCharacter = Cast<AGamerCharacter>(GetPawn());
	AGamerSettlement* Settlement = AGamerSettlement::Get(this);
	if (!GamerCharacter || !Settlement || !StorageInReach())
	{
		return;
	}
	UGamerInventoryComponent* Inventory = GamerCharacter->GetInventory();
	const int32 Have = FMath::Min(Count, Inventory->CountOf(Item));
	if (Have <= 0)
	{
		return;
	}
	const int32 NotFitting = Settlement->AddToStorage(Item, Have);
	const int32 Stored = Have - NotFitting;
	if (Stored > 0)
	{
		FGamerItemAmount Amount;
		Amount.Item = Item;
		Amount.Count = Stored;
		Inventory->RemoveItems({ Amount });
	}
	if (NotFitting > 0)
	{
		GamerCharacter->GetWork()->ClientMessage(TEXT("Das Lager ist voll. Baue ein weiteres Lager."));
	}
	UE_LOG(LogGamer, Log, TEXT("Storage deposit: %s x%d - %s"), *Item.ToString(), Stored, *Settlement->Describe());
}

void AGamerPlayerController::ServerStorageDepositAll_Implementation(bool bIncludeTools)
{
	AGamerCharacter* GamerCharacter = Cast<AGamerCharacter>(GetPawn());
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (!GamerCharacter || !Data)
	{
		return;
	}
	TSet<FName> Items;
	for (const FGamerInventorySlot& Slot : GamerCharacter->GetInventory()->GetSlots())
	{
		const FGamerItemDef* Def = Slot.IsEmpty() ? nullptr : Data->FindItem(Slot.Item);
		if (Def && (bIncludeTools || Def->Category != EGamerItemCategory::Tool))
		{
			Items.Add(Slot.Item);
		}
	}
	for (const FName Item : Items)
	{
		ServerStorageDeposit_Implementation(Item, 9999);
	}
}

void AGamerPlayerController::ServerSetVillagerJob_Implementation(AGamerVillager* Villager, FName JobId)
{
	// The village management works from anywhere (no reach check): it is the settlement's job board.
	if (Villager && GetPawn())
	{
		Villager->SetJob(JobId);
	}
}

void AGamerPlayerController::ServerDebugSpawnVillager_Implementation()
{
#if !UE_BUILD_SHIPPING
	AGamerSettlement* Settlement = AGamerSettlement::Get(this);
	if (Settlement && GetPawn())
	{
		Settlement->SpawnVillager(GetPawn()->GetActorLocation() + GetPawn()->GetActorForwardVector() * 250.f);
	}
#endif
}

void AGamerPlayerController::ApplyKeyBindingsToPawn()
{
	if (AGamerCharacter* GamerCharacter = Cast<AGamerCharacter>(GetPawn()))
	{
		GamerCharacter->ApplyKeyBindings();
	}
}

void AGamerPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}
	bInStartMenu = GetWorld()->GetAuthGameMode<AGamerMenuGameMode>() != nullptr;
	if (bInStartMenu)
	{
		JoinAddress = UGamerMenuSettings::Get()->JoinAddress;
		OpenMenu(EGamerMenuPage::Start);
		static bool bMenuScriptLoaded = false; // only once per process: coming back to the menu must not repeat it
		if (!bMenuScriptLoaded)
		{
			bMenuScriptLoaded = true;
			ParseScript(TEXT("GamerMenuScript="));
		}
		return;
	}

	// The start menu frees the mouse (captured only while a button is held). That setting belongs to the game
	// window and survives the map change, so in the world the mouse has to be taken back for looking around.
	SetShowMouseCursor(false);
	SetInputMode(FInputModeGameOnly());
	if (const UGameViewportClient* Viewport = GetWorld()->GetGameViewport())
	{
		UE_LOG(LogGamer, Log, TEXT("World input: game only, mouse capture mode %d, player %s"),
			static_cast<int32>(Viewport->GetMouseCaptureMode()), Player ? TEXT("set") : TEXT("missing"));
	}

	FParse::Value(FCommandLine::Get(), TEXT("GamerTestWalk="), PendingWalkSeconds);
	FParse::Value(FCommandLine::Get(), TEXT("GamerTestWalkDelay="), PendingWalkDelay);
	ParseScript(TEXT("GamerTestScript="));
}

void AGamerPlayerController::ParseScript(const TCHAR* CommandLineKey)
{
	{
		FString Script;
		if (FParse::Value(FCommandLine::Get(), CommandLineKey, Script, false))
		{
			TArray<FString> Parts;
			Script.ParseIntoArray(Parts, TEXT(";"));
			for (const FString& Part : Parts)
			{
				FString Step = Part.TrimStartAndEnd();
				int32 Repeat = 1;
				FString Base, Count;
				if (Step.Split(TEXT("*"), &Base, &Count))
				{
					Step = Base;
					Repeat = FMath::Max(1, FCString::Atoi(*Count));
				}
				for (int32 Index = 0; Index < Repeat; ++Index)
				{
					TestSteps.Add(Step);
				}
			}
			TestPause = 3.f; // let the other players join and the start area replicate
			UE_LOG(LogGamer, Log, TEXT("Test script: %d steps"), TestSteps.Num());
		}
	}
}

void AGamerPlayerController::RunMenuScript(float DeltaTime)
{
	if (!TestSteps.IsValidIndex(TestStepIndex))
	{
		return;
	}
	if (TestPause > 0.f)
	{
		TestPause -= DeltaTime;
		return;
	}
	const FString Step = TestSteps[TestStepIndex++];
	FString Kind, Argument;
	if (!Step.Split(TEXT(":"), &Kind, &Argument))
	{
		Kind = Step;
	}
	UE_LOG(LogGamer, Log, TEXT("Menu step %d/%d: %s"), TestStepIndex, TestSteps.Num(), *Step);
	if (Kind == TEXT("wait"))
	{
		TestPause = FCString::Atof(*Argument);
		return;
	}
	if (Kind == TEXT("button"))
	{
		PressMenuButton(FName(*Argument));
	}
	else if (Kind == TEXT("press"))
	{
		const FKey Key{ FName(*Argument) };
		InputKey(FInputKeyEventArgs::CreateSimulated(Key, IE_Pressed, 1.f));
		InputKey(FInputKeyEventArgs::CreateSimulated(Key, IE_Released, 0.f));
	}
	else if (Kind == TEXT("address"))
	{
		JoinAddress = Argument;
	}
	else if (Kind == TEXT("shot"))
	{
		TakeTestShot();
	}
	UE_LOG(LogGamer, Log, TEXT("Menu state: page %d, address \"%s\", note \"%s\""), static_cast<int32>(MenuPage), *JoinAddress, *MenuNote);
	TestPause = 1.f;
}

void AGamerPlayerController::TakeTestShot()
{
	static int32 ShotNumber = 0;
	const FString File = FPaths::Combine(FPaths::ScreenShotDir(), FString::Printf(TEXT("gamer_test_%d.png"), ++ShotNumber));
	FScreenshotRequest::RequestScreenshot(File, true, false);
	UE_LOG(LogGamer, Log, TEXT("Screenshot requested: %s"), *FPaths::ConvertRelativePathToFull(File));
	GLog->Flush(); // outside tools watch the log to capture the window at this moment
}

void AGamerPlayerController::RunTestScript(float DeltaTime)
{
	if (bInStartMenu)
	{
		RunMenuScript(DeltaTime);
		return;
	}
	AGamerCharacter* GamerCharacter = Cast<AGamerCharacter>(GetPawn());
	// Keep running while the last step is still striking a node.
	if (!GamerCharacter || (!TestSteps.IsValidIndex(TestStepIndex) && !StrikeTarget.IsValid()))
	{
		return;
	}
	if (TestPause > 0.f)
	{
		TestPause -= DeltaTime;
		return;
	}
	UGamerWorkComponent* Work = GamerCharacter->GetWork();

	// A tree, rock or building plan: keep striking until it is done (or give up after the timeout).
	// "hammer" moves on to the next unfinished plan until none is left.
	if (StrikeTarget.IsValid() || StrikeTimeLeft > 0.f)
	{
		AActor* Target = StrikeTarget.Get();
		AGamerResourceNode* Node = Cast<AGamerResourceNode>(Target);
		AGamerBuilding* Building = Cast<AGamerBuilding>(Target);
		const bool bDone = !Target || (Node && Node->IsDepleted()) || (Building && Building->IsFinished());
		StrikeTimeLeft -= DeltaTime;
		if (bDone || StrikeTimeLeft <= 0.f)
		{
			if (!bDone)
			{
				UE_LOG(LogGamer, Warning, TEXT("Test step: gave up striking %s"), *Target->GetName());
			}
			StrikeTarget.Reset();
			StrikeTimeLeft = 0.f;
			if (bHammerAll && bDone)
			{
				StartHammerNext();
				return;
			}
			bHammerAll = false;
			TestPause = 1.f;
			return;
		}
		StrikeCooldown -= DeltaTime;
		if (StrikeCooldown <= 0.f)
		{
			if (Node)
			{
				Work->ServerStrike(Node);
			}
			else if (Building)
			{
				GamerCharacter->GetBuild()->ServerHammer(Building);
			}
			StrikeCooldown = Work->GetStrikeInterval() + 0.05f;
		}
		return;
	}

	if (Work->IsBusy() || !TestSteps.IsValidIndex(TestStepIndex))
	{
		return;
	}

	const FString Step = TestSteps[TestStepIndex++];
	FString Kind, Argument;
	if (!Step.Split(TEXT(":"), &Kind, &Argument))
	{
		Kind = Step; // steps without an argument: keys, resetkeys, shot
	}
	UE_LOG(LogGamer, Log, TEXT("Test step %d/%d: %s"), TestStepIndex, TestSteps.Num(), *Step);

	if (Kind == TEXT("harvest"))
	{
		AGamerResourceNode* Nearest = nullptr;
		float BestDistance = TNumericLimits<float>::Max();
		for (TActorIterator<AGamerResourceNode> It(GetWorld()); It; ++It)
		{
			if (It->GetNodeId() == FName(*Argument) && !It->IsDepleted())
			{
				const float Distance = FVector::Dist(It->GetActorLocation(), GamerCharacter->GetActorLocation());
				if (Distance < BestDistance)
				{
					BestDistance = Distance;
					Nearest = *It;
				}
			}
		}
		if (!Nearest)
		{
			UE_LOG(LogGamer, Warning, TEXT("Test step: no full node %s found"), *Argument);
			return;
		}
		Work->ServerDebugTeleportNear(Nearest);
		const FGamerResourceNodeDef* Def = Nearest->GetDef();
		if (Def && Def->RequiredTool.IsRequired())
		{
			// Like a player: take the matching tool from the hotbar into the hand, then strike.
			UGamerInventoryComponent* Inventory = GamerCharacter->GetInventory();
			const int32 Slot = Inventory->FindToolInHotbar(Def->RequiredTool.Type, Def->RequiredTool.MinTier);
			if (Slot != INDEX_NONE)
			{
				Inventory->SelectSlot(Slot);
				UE_LOG(LogGamer, Log, TEXT("Test step: hotbar slot %d selected"), Slot + 1);
			}
			else
			{
				UE_LOG(LogGamer, Warning, TEXT("Test step: no %s in the hotbar, strikes will be refused"), *Def->RequiredTool.Type.ToString());
			}
			StrikeTarget = Nearest;
			StrikeTimeLeft = 60.f;
			StrikeCooldown = 0.5f; // let the teleport and the slot selection arrive first
			return;
		}
		Work->ServerHarvest(Nearest);
	}
	else if (Kind == TEXT("handharvest"))
	{
		// Refusal test: try the interact key on the nearest node of that type (trees and rocks must say "Schlagen").
		for (TActorIterator<AGamerResourceNode> It(GetWorld()); It; ++It)
		{
			if (It->GetNodeId() == FName(*Argument) && !It->IsDepleted())
			{
				Work->ServerDebugTeleportNear(*It);
				Work->ServerHarvest(*It);
				break;
			}
		}
	}
	else if (Kind == TEXT("select"))
	{
		GamerCharacter->GetInventory()->SelectSlot(FCString::Atoi(*Argument) - 1);
	}
	else if (Kind == TEXT("bind"))
	{
		// Same path as the menu: bind:interact=F
		FString Action, KeyName;
		if (Argument.Split(TEXT("="), &Action, &KeyName))
		{
			UGamerKeyBindings::Get()->SetKey(FName(*Action), FKey(FName(*KeyName)));
			ApplyKeyBindingsToPawn();
		}
		UE_LOG(LogGamer, Log, TEXT("Key mapping: %s"), *GamerCharacter->DescribeKeyBindings());
	}
	else if (Kind == TEXT("resetkeys"))
	{
		UGamerKeyBindings::Get()->ResetToDefaults();
		ApplyKeyBindingsToPawn();
		UE_LOG(LogGamer, Log, TEXT("Key mapping: %s"), *GamerCharacter->DescribeKeyBindings());
	}
	else if (Kind == TEXT("keys"))
	{
		UE_LOG(LogGamer, Log, TEXT("Key mapping: %s"), *GamerCharacter->DescribeKeyBindings());
	}
	else if (Kind == TEXT("menu"))
	{
		if (Argument == TEXT("close"))
		{
			CloseMenu();
		}
		else
		{
			const EGamerMenuPage Page = Argument == TEXT("controls") ? EGamerMenuPage::Controls
				: Argument == TEXT("village") ? EGamerMenuPage::Village
				: Argument == TEXT("storage") ? EGamerMenuPage::Storage
				: Argument == TEXT("graphics") ? EGamerMenuPage::Graphics
				: Argument == TEXT("build") ? EGamerMenuPage::Build : EGamerMenuPage::Main;
			OpenMenu(Page);
		}
	}
	else if (Kind == TEXT("button"))
	{
		// A menu button as if clicked: button:controls, button:bind:strike, button:reset
		PressMenuButton(FName(*Argument));
	}
	else if (Kind == TEXT("press"))
	{
		// A key press through the same input path as a real key: press:Escape, press:R
		const FKey Key{ FName(*Argument) };
		InputKey(FInputKeyEventArgs::CreateSimulated(Key, IE_Pressed, 1.f));
		InputKey(FInputKeyEventArgs::CreateSimulated(Key, IE_Released, 0.f));
		UE_LOG(LogGamer, Log, TEXT("Test press %s: menu page %d, listening %s, note \"%s\""),
			*Argument, static_cast<int32>(MenuPage), *ListeningAction.ToString(), *MenuNote);
	}
	else if (Kind == TEXT("place"))
	{
		// place:<buildable>@x,y[,z]
		FString Id, Coords;
		Argument.Split(TEXT("@"), &Id, &Coords);
		TArray<FString> Parts;
		Coords.ParseIntoArray(Parts, TEXT(","));
		const FVector Local(Parts.IsValidIndex(0) ? FCString::Atof(*Parts[0]) : 0.f, Parts.IsValidIndex(1) ? FCString::Atof(*Parts[1]) : 0.f,
			Parts.IsValidIndex(2) ? FCString::Atof(*Parts[2]) : 0.f);

		UGamerInventoryComponent* Inventory = GamerCharacter->GetInventory();
		const int32 HammerSlot = Inventory->FindToolInHotbar(TEXT("hammer"), 1);
		if (HammerSlot != INDEX_NONE)
		{
			Inventory->SelectSlot(HammerSlot);
		}
		GamerCharacter->GetBuild()->SelectBuildable(FName(*Id));

		AGamerBuilding* Frame = nullptr;
		float BestDistance = TNumericLimits<float>::Max();
		for (TActorIterator<AGamerBuilding> It(GetWorld()); It; ++It)
		{
			const FGamerBuildableDef* Def = It->IsPreview() ? nullptr : It->GetDef();
			const float Distance = FVector::Dist(It->GetActorLocation(), GamerCharacter->GetActorLocation());
			if (Def && Def->Snap == TEXT("foundation") && Distance < BestDistance)
			{
				BestDistance = Distance;
				Frame = *It;
			}
		}
		FVector Aim;
		bool bHit = true;
		if (Frame)
		{
			Aim = Frame->GetActorTransform().TransformPosition(Local);
		}
		else
		{
			const FRotator Yaw(0.f, GetControlRotation().Yaw, 0.f);
			const FVector Ahead = GamerCharacter->GetActorLocation() + Yaw.RotateVector(FVector(Local.X, Local.Y, 0.f));
			FHitResult Hit;
			FCollisionQueryParams Params(SCENE_QUERY_STAT(GamerTestPlace), false, GamerCharacter);
			bHit = GetWorld()->LineTraceSingleByChannel(Hit, Ahead + FVector(0, 0, 500.f), Ahead - FVector(0, 0, 2000.f), ECC_Visibility, Params);
			Aim = (bHit ? Hit.ImpactPoint : Ahead) + FVector(0, 0, Local.Z);
		}
		const FGamerPlacement Placement = GamerCharacter->GetBuild()->ComputePlacement(FName(*Id), Aim, bHit, GetControlRotation().Yaw);
		UE_LOG(LogGamer, Log, TEXT("Test place %s: %s%s at %s yaw %.0f%s"), *Id, Placement.bValid ? TEXT("valid") : TEXT("INVALID"),
			Placement.bSnapped ? TEXT(" (snapped)") : TEXT(""), *Placement.Transform.GetLocation().ToCompactString(),
			Placement.Transform.Rotator().Yaw, Placement.Problem.IsEmpty() ? TEXT("") : *(TEXT(" - ") + Placement.Problem));
		if (Placement.bValid)
		{
			GamerCharacter->GetBuild()->ServerPlace(FName(*Id), Placement.Transform);
		}
	}
	else if (Kind == TEXT("give"))
	{
		// give:item_plank:50 ("*" is the repeat syntax; "=" would clash with Unreal's own -log= option for item_log)
		FString Item, Count;
		if (!Argument.Split(TEXT(":"), &Item, &Count))
		{
			Item = Argument;
			Count = TEXT("1");
		}
		Work->ServerDebugGiveItem(FName(*Item), FCString::Atoi(*Count));
	}
	else if (Kind == TEXT("hammer"))
	{
		UGamerInventoryComponent* Inventory = GamerCharacter->GetInventory();
		const int32 HammerSlot = Inventory->FindToolInHotbar(TEXT("hammer"), 1);
		if (HammerSlot != INDEX_NONE)
		{
			Inventory->SelectSlot(HammerSlot);
		}
		bHammerAll = true;
		HammerTimeout = Argument.IsEmpty() ? 60.f : FCString::Atof(*Argument); // hammer:<seconds per plan>
		StartHammerNext();
		return;
	}
	else if (Kind == TEXT("demolish") || Kind == TEXT("goto"))
	{
		AGamerBuilding* Nearest = nullptr;
		float BestDistance = TNumericLimits<float>::Max();
		for (TActorIterator<AGamerBuilding> It(GetWorld()); It; ++It)
		{
			const float Distance = FVector::Dist(It->GetActorLocation(), GamerCharacter->GetActorLocation());
			if (!It->IsPreview() && It->GetBuildableId() == FName(*Argument) && Distance < BestDistance)
			{
				BestDistance = Distance;
				Nearest = *It;
			}
		}
		if (!Nearest)
		{
			UE_LOG(LogGamer, Warning, TEXT("Test step: no building %s found"), *Argument);
		}
		else
		{
			Work->ServerDebugTeleportNear(Nearest);
			if (Kind == TEXT("demolish"))
			{
				UGamerInventoryComponent* Inventory = GamerCharacter->GetInventory();
				const int32 HammerSlot = Inventory->FindToolInHotbar(TEXT("hammer"), 1);
				if (HammerSlot != INDEX_NONE)
				{
					Inventory->SelectSlot(HammerSlot);
				}
				GamerCharacter->GetBuild()->ServerDemolish(Nearest);
			}
		}
	}
	else if (Kind == TEXT("view"))
	{
		// view:<buildable>:<distance>[:<angle>]: stand that far in front of the first such piece (angle in degrees
		// around it, 0 = its -X side) and look at it.
		TArray<FString> Parts;
		Argument.ParseIntoArray(Parts, TEXT(":"));
		const FString Id = Parts.IsValidIndex(0) ? Parts[0] : FString();
		const float Distance = Parts.IsValidIndex(1) ? FCString::Atof(*Parts[1]) : 900.f;
		const float Angle = Parts.IsValidIndex(2) ? FCString::Atof(*Parts[2]) : 0.f;
		AGamerBuilding* Target = nullptr;
		for (TActorIterator<AGamerBuilding> It(GetWorld()); It; ++It)
		{
			if (!It->IsPreview() && It->GetBuildableId() == FName(*Id))
			{
				Target = *It;
				break;
			}
		}
		if (Target)
		{
			// In front of the piece's -X side (for a foundation placed by the test: the door side).
			FVector Away = FRotator(0.f, Angle, 0.f).RotateVector(-Target->GetActorForwardVector());
			Away.Z = 0.f;
			Away = Away.GetSafeNormal();
			FVector Spot = Target->GetActorLocation() + Away * Distance;
			Spot.Z = GamerCharacter->GetActorLocation().Z;
			Work->ServerDebugTeleportTo(Spot);
			const FVector LookAt = Target->GetActorLocation() + FVector(0.f, 0.f, 150.f);
			SetControlRotation((LookAt - (Spot + FVector(0.f, 0.f, 150.f))).Rotation());
		}
	}
	else if (Kind == TEXT("deposit") || Kind == TEXT("take"))
	{
		FString Item, Count;
		if (!Argument.Split(TEXT(":"), &Item, &Count))
		{
			Item = Argument;
			Count = TEXT("9999");
		}
		if (Kind == TEXT("deposit"))
		{
			ServerStorageDeposit(FName(*Item), FCString::Atoi(*Count));
		}
		else
		{
			ServerStorageTake(FName(*Item), FCString::Atoi(*Count));
		}
	}
	else if (Kind == TEXT("depositall"))
	{
		ServerStorageDepositAll(true);
	}
	else if (Kind == TEXT("spawnvillager"))
	{
		ServerDebugSpawnVillager();
	}
	else if (Kind == TEXT("setjob"))
	{
		FString Index, JobName;
		Argument.Split(TEXT(":"), &Index, &JobName);
		const TArray<AGamerVillager*> Villagers = GetVillagersSorted();
		const int32 N = FCString::Atoi(*Index) - 1;
		if (Villagers.IsValidIndex(N))
		{
			ServerSetVillagerJob(Villagers[N], FName(*JobName));
		}
		else
		{
			UE_LOG(LogGamer, Warning, TEXT("Test step: no villager %s (have %d)"), *Index, Villagers.Num());
		}
	}
	else if (Kind == TEXT("villagers"))
	{
		for (const AGamerVillager* Villager : GetVillagersSorted())
		{
			UE_LOG(LogGamer, Log, TEXT("Villager seen: %s job=%s activity=\"%s\" hunger=%.2f sleep=%.2f shelter=%d bed=%d tool=%s"),
				*Villager->GetVillagerName(), *Villager->GetJobId().ToString(), *Villager->GetActivity(), Villager->GetHunger(), Villager->GetSleep(),
				Villager->HasShelter() ? 1 : 0, Villager->GetBed() ? 1 : 0, *Villager->GetHeldTool().ToString());
		}
		UE_LOG(LogGamer, Log, TEXT("Villagers seen by this player: %d"), GetVillagersSorted().Num());
	}
	else if (Kind == TEXT("settlement"))
	{
		const AGamerSettlement* Settlement = AGamerSettlement::Get(this);
		UE_LOG(LogGamer, Log, TEXT("Settlement seen: %s"), Settlement ? *Settlement->Describe() : TEXT("(none)"));
	}
	else if (Kind == TEXT("graphics"))
	{
		UE_LOG(LogGamer, Log, TEXT("Graphics seen: %s"), *GamerGraphics::Describe());
		for (const FGamerGraphicsRow& Row : GamerGraphics::GetRows())
		{
			UE_LOG(LogGamer, Log, TEXT("Graphics row %s: %s = %s%s"), *Row.Id.ToString(), *Row.Label, *Row.Value, Row.bEnabled ? TEXT("") : TEXT(" (disabled)"));
		}
	}
	else if (Kind == TEXT("bird"))
	{
		SetBirdView(Argument != TEXT("off"));
	}
	else if (Kind == TEXT("birdfocus") || Kind == TEXT("birdselect"))
	{
		// <target> = villager:<n> (name order) or a buildable id (the nearest one).
		AActor* Target = nullptr;
		if (Argument.StartsWith(TEXT("villager:")))
		{
			const TArray<AGamerVillager*> Villagers = GetVillagersSorted();
			const int32 N = FCString::Atoi(*Argument.Mid(9)) - 1;
			Target = Villagers.IsValidIndex(N) ? Villagers[N] : nullptr;
		}
		else
		{
			float BestDistance = TNumericLimits<float>::Max();
			for (TActorIterator<AGamerBuilding> It(GetWorld()); It; ++It)
			{
				const float Distance = FVector::Dist(It->GetActorLocation(), GamerCharacter->GetActorLocation());
				if (!It->IsPreview() && It->GetBuildableId() == FName(*Argument) && Distance < BestDistance)
				{
					BestDistance = Distance;
					Target = *It;
				}
			}
		}
		if (!Target)
		{
			UE_LOG(LogGamer, Warning, TEXT("Test %s: no target %s"), *Kind, *Argument);
		}
		else if (Kind == TEXT("birdfocus"))
		{
			BirdFocus = Target->GetActorLocation();
			ApplyBirdCamera();
		}
		else
		{
			// Click where the target is on screen, through the same picking as the mouse.
			FVector2D OnScreen;
			const bool bOnScreen = ProjectWorldLocationToScreen(Target->GetActorLocation(), OnScreen, true);
			AActor* Picked = bOnScreen ? FindBirdTargetAt(OnScreen) : nullptr;
			UE_LOG(LogGamer, Log, TEXT("Test birdselect %s: on screen %d at %s, picked %s"), *Argument, bOnScreen ? 1 : 0,
				*OnScreen.ToString(), Picked == Target ? TEXT("the target") : *GetNameSafe(Picked));
			SelectBirdTarget(Picked);
		}
	}
	else if (Kind == TEXT("birdzoom"))
	{
		BirdDistance = FMath::Clamp(FCString::Atof(*Argument), 800.f, 8000.f);
	}
	else if (Kind == TEXT("birdturn"))
	{
		FString Yaw, Pitch;
		Argument.Split(TEXT(","), &Yaw, &Pitch);
		BirdYaw = FCString::Atof(*Yaw);
		BirdPitch = FMath::Clamp(FCString::Atof(*Pitch), -85.f, -25.f);
	}
	else if (Kind == TEXT("birdinfo"))
	{
		const AActor* ViewTarget = GetViewTarget();
		UE_LOG(LogGamer, Log, TEXT("Bird view: %d, view target %s, camera %s, focus %s, distance %.0f, yaw %.0f, pitch %.0f, cursor %d"),
			bBirdView ? 1 : 0, *GetNameSafe(ViewTarget), BirdCamera ? *BirdCamera->GetActorLocation().ToCompactString() : TEXT("-"),
			*BirdFocus.ToCompactString(), BirdDistance, BirdYaw, BirdPitch, ShouldShowMouseCursor() ? 1 : 0);
		const TArray<FString> Lines = DescribeBirdSelection();
		UE_LOG(LogGamer, Log, TEXT("Bird view selection: %s"), Lines.Num() > 0 ? *FString::Join(Lines, TEXT(" | ")) : TEXT("(nothing)"));
	}
	else if (Kind == TEXT("buildings"))
	{
		int32 Count = 0;
		for (TActorIterator<AGamerBuilding> It(GetWorld()); It; ++It)
		{
			if (!It->IsPreview())
			{
				UE_LOG(LogGamer, Log, TEXT("Building %s: %s progress %.2f at %s yaw %.0f"), *It->GetName(), *It->GetBuildableId().ToString(),
					It->GetProgress(), *It->GetActorLocation().ToCompactString(), It->GetActorRotation().Yaw);
				++Count;
			}
		}
		UE_LOG(LogGamer, Log, TEXT("Buildings seen by this player: %d"), Count);
	}
	else if (Kind == TEXT("shot"))
	{
		TakeTestShot();
	}
	else if (Kind == TEXT("craft"))
	{
		Work->ServerCraft(FName(*Argument));
	}
	else if (Kind == TEXT("wait"))
	{
		TestPause = FCString::Atof(*Argument);
		return;
	}
	// Give the server time to start the work and replicate "busy" before checking again.
	TestPause = 1.5f;
}

void AGamerPlayerController::StartHammerNext()
{
	AGamerCharacter* GamerCharacter = Cast<AGamerCharacter>(GetPawn());
	AGamerBuilding* Next = nullptr;
	float BestDistance = 3000.f;
	for (TActorIterator<AGamerBuilding> It(GetWorld()); It && GamerCharacter; ++It)
	{
		const float Distance = FVector::Dist(It->GetActorLocation(), GamerCharacter->GetActorLocation());
		if (!It->IsPreview() && !It->IsFinished() && Distance < BestDistance)
		{
			BestDistance = Distance;
			Next = *It;
		}
	}
	if (!Next)
	{
		UE_LOG(LogGamer, Log, TEXT("Test hammer: no unfinished plans left"));
		bHammerAll = false;
		TestPause = 1.f;
		return;
	}
	if (BestDistance > 400.f)
	{
		GamerCharacter->GetWork()->ServerDebugTeleportNear(Next);
	}
	StrikeTarget = Next;
	StrikeTimeLeft = HammerTimeout;
	StrikeCooldown = 0.5f;
}

void AGamerPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	RunTestScript(DeltaTime);
	UpdateBirdView(DeltaTime);

	if (PendingWalkSeconds <= 0.f)
	{
		return;
	}
	AGamerCharacter* GamerCharacter = Cast<AGamerCharacter>(GetPawn());
	if (!GamerCharacter)
	{
		return; // wait until the server has given us a pawn
	}
	PendingWalkDelay -= DeltaTime;
	if (PendingWalkDelay <= 0.f)
	{
		GamerCharacter->StartTestWalk(PendingWalkSeconds);
		PendingWalkSeconds = 0.f;
	}
}

void AGamerPlayerController::GamerTestWalk(float Seconds)
{
	if (AGamerCharacter* GamerCharacter = Cast<AGamerCharacter>(GetPawn()))
	{
		GamerCharacter->StartTestWalk(Seconds);
	}
	else
	{
		UE_LOG(LogGamer, Warning, TEXT("GamerTestWalk: no GamerCharacter possessed."));
	}
}

// --- Bird's-eye view -------------------------------------------------------------------------------

namespace
{
	constexpr float BirdMaxRange = 12000.f;     // the camera's focus stays within this distance (cm) of the own character
	constexpr float BirdBlendSeconds = 0.35f;
	constexpr float BirdPanSpeed = 1.1f;        // per second, in units of the camera distance
	constexpr float BirdTurnPerPixel = 0.3f;    // degrees of yaw per pixel of mouse movement
	constexpr float BirdTiltPerPixel = 0.2f;
	constexpr float BirdPickPixels = 45.f;      // a villager counts as clicked this close on screen (at 1080 p)
}

void AGamerPlayerController::SetBirdView(bool bOn)
{
	APawn* MyPawn = GetPawn();
	if (bOn == bBirdView || !IsLocalController() || (bOn && (!MyPawn || bInStartMenu)))
	{
		return;
	}
	bBirdView = bOn;
	bBirdRotating = false;
	BirdSelection.Reset();
	FlushPressedKeys();
	if (bOn)
	{
		if (!BirdCamera)
		{
			// Local only: an actor spawned without replication exists just for this player.
			BirdCamera = GetWorld()->SpawnActor<ACameraActor>();
			BirdCamera->GetCameraComponent()->bConstrainAspectRatio = false;
			BirdCamera->GetCameraComponent()->SetFieldOfView(75.f);
		}
		BirdFocus = MyPawn->GetActorLocation();
		BirdYaw = GetControlRotation().Yaw;
		BirdPitch = -55.f;
		BirdDistance = 2600.f;
		ApplyBirdCamera();
		SetViewTargetWithBlend(BirdCamera, BirdBlendSeconds, VTBlend_Cubic);
	}
	else
	{
		SetViewTargetWithBlend(MyPawn, BirdBlendSeconds, VTBlend_Cubic);
	}
	ApplyInputModeForState();
	UE_LOG(LogGamer, Log, TEXT("Bird view %s"), bOn ? TEXT("on") : TEXT("off"));
}

void AGamerPlayerController::ApplyBirdCamera()
{
	if (BirdCamera)
	{
		const FRotator Rotation(BirdPitch, BirdYaw, 0.f);
		BirdCamera->SetActorLocationAndRotation(BirdFocus - Rotation.Vector() * BirdDistance, Rotation);
	}
}

void AGamerPlayerController::UpdateBirdView(float DeltaTime)
{
	if (!bBirdView)
	{
		return;
	}
	const APawn* MyPawn = GetPawn();
	if (!MyPawn || !BirdCamera)
	{
		SetBirdView(false);
		return;
	}
	if (!IsMenuOpen())
	{
		// Pan with the move keys, relative to where the camera looks.
		const UGamerKeyBindings* Keys = UGamerKeyBindings::Get();
		FVector Pan(0.f, 0.f, 0.f);
		Pan.X += IsInputKeyDown(Keys->GetKey(GamerInputActions::MoveForward)) ? 1.f : 0.f;
		Pan.X -= IsInputKeyDown(Keys->GetKey(GamerInputActions::MoveBack)) ? 1.f : 0.f;
		Pan.Y += IsInputKeyDown(Keys->GetKey(GamerInputActions::MoveRight)) ? 1.f : 0.f;
		Pan.Y -= IsInputKeyDown(Keys->GetKey(GamerInputActions::MoveLeft)) ? 1.f : 0.f;
		if (!Pan.IsNearlyZero())
		{
			BirdFocus += FRotator(0.f, BirdYaw, 0.f).RotateVector(Pan).GetSafeNormal2D() * BirdDistance * BirdPanSpeed * DeltaTime;
		}
		// Turn and tilt with the right mouse button held (cursor movement in pixels).
		float MouseX = 0.f, MouseY = 0.f;
		if (IsInputKeyDown(EKeys::RightMouseButton) && GetMousePosition(MouseX, MouseY))
		{
			if (bBirdRotating)
			{
				BirdYaw += (MouseX - BirdLastMouse.X) * BirdTurnPerPixel;
				BirdPitch = FMath::Clamp(BirdPitch - (MouseY - BirdLastMouse.Y) * BirdTiltPerPixel, -85.f, -25.f);
			}
			bBirdRotating = true;
			BirdLastMouse = FVector2D(MouseX, MouseY);
		}
		else
		{
			bBirdRotating = false;
		}
	}

	// Stay near the own character (no free scouting camera in co-op).
	const FVector PawnLocation = MyPawn->GetActorLocation();
	FVector Offset = BirdFocus - PawnLocation;
	Offset.Z = 0.f;
	if (Offset.Size() > BirdMaxRange)
	{
		Offset = Offset.GetSafeNormal() * BirdMaxRange;
		BirdFocus.X = PawnLocation.X + Offset.X;
		BirdFocus.Y = PawnLocation.Y + Offset.Y;
	}
	// Follow the ground height under the focus point, smoothly (roofs and trees only nudge it).
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(GamerBirdGround), false, MyPawn);
	const FVector Top(BirdFocus.X, BirdFocus.Y, PawnLocation.Z + 5000.f);
	const FVector Bottom(BirdFocus.X, BirdFocus.Y, PawnLocation.Z - 5000.f);
	const float GroundZ = GetWorld()->LineTraceSingleByChannel(Hit, Top, Bottom, ECC_Visibility, Params) ? Hit.ImpactPoint.Z : PawnLocation.Z;
	BirdFocus.Z = FMath::FInterpTo(BirdFocus.Z, GroundZ, DeltaTime, 2.f);
	ApplyBirdCamera();
}

AActor* AGamerPlayerController::FindBirdTargetAt(const FVector2D& ScreenPosition) const
{
	int32 ViewX = 0, ViewY = 0;
	GetViewportSize(ViewX, ViewY);
	float BestDistance = BirdPickPixels * FMath::Max(1.f, ViewY / 1080.f);
	AActor* Best = nullptr;
	for (TActorIterator<AGamerVillager> It(GetWorld()); It; ++It)
	{
		FVector2D OnScreen;
		if (ProjectWorldLocationToScreen(It->GetActorLocation(), OnScreen, true))
		{
			const float Distance = FVector2D::Distance(OnScreen, ScreenPosition);
			if (Distance < BestDistance)
			{
				BestDistance = Distance;
				Best = *It;
			}
		}
	}
	if (Best)
	{
		return Best;
	}
	FVector WorldOrigin, WorldDirection;
	if (DeprojectScreenPositionToWorld(ScreenPosition.X, ScreenPosition.Y, WorldOrigin, WorldDirection))
	{
		FHitResult Hit;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(GamerBirdPick), false, GetPawn());
		if (GetWorld()->LineTraceSingleByChannel(Hit, WorldOrigin, WorldOrigin + WorldDirection * 30000.f, ECC_Visibility, Params))
		{
			AActor* HitActor = Hit.GetActor();
			const AGamerBuilding* Building = Cast<AGamerBuilding>(HitActor);
			if ((Building && !Building->IsPreview()) || Cast<AGamerResourceNode>(HitActor) || Cast<AGamerVillager>(HitActor))
			{
				return HitActor;
			}
		}
	}
	return nullptr;
}

void AGamerPlayerController::SelectBirdTarget(AActor* Target)
{
	BirdSelection = Target;
	const TArray<FString> Lines = DescribeBirdSelection();
	UE_LOG(LogGamer, Log, TEXT("Bird view selection: %s"), Lines.Num() > 0 ? *FString::Join(Lines, TEXT(" | ")) : TEXT("(nothing)"));
}

TArray<FString> AGamerPlayerController::DescribeBirdSelection() const
{
	TArray<FString> Lines;
	const AActor* Target = BirdSelection.Get();
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (const AGamerVillager* Villager = Cast<AGamerVillager>(Target))
	{
		const FGamerJobDef* Job = Data ? Data->FindJob(Villager->GetJobId()) : nullptr;
		const FGamerItemDef* Tool = Data && !Villager->GetHeldTool().IsNone() ? Data->FindItem(Villager->GetHeldTool()) : nullptr;
		Lines.Add(Villager->GetVillagerName());
		Lines.Add(FString::Printf(TEXT("Beruf: %s"), Job ? *Job->Name : TEXT("-")));
		Lines.Add(FString::Printf(TEXT("Tätigkeit: %s"), *Villager->GetActivity()));
		Lines.Add(FString::Printf(TEXT("Hunger: %d %%   Schlaf: %d %%"), FMath::RoundToInt(Villager->GetHunger() * 100.f), FMath::RoundToInt(Villager->GetSleep() * 100.f)));
		Lines.Add(FString::Printf(TEXT("Unterkunft: %s"), !Villager->GetBed() ? TEXT("kein Bett") : Villager->HasShelter() ? TEXT("ja") : TEXT("Bett ohne Dach")));
		Lines.Add(FString::Printf(TEXT("Werkzeug: %s"), Tool ? *Tool->Name : TEXT("keins")));
	}
	else if (const AGamerBuilding* Building = Cast<AGamerBuilding>(Target))
	{
		const FGamerBuildableDef* Def = Building->GetDef();
		Lines.Add(Def ? Def->Name : Building->GetBuildableId().ToString());
		Lines.Add(Building->IsFinished() ? FString(TEXT("fertig")) : FString::Printf(TEXT("Bauplan: %d %%"), FMath::RoundToInt(Building->GetProgress() * 100.f)));
		const AGamerSettlement* Settlement = AGamerSettlement::Get(this);
		if (Def && Def->Category == EGamerBuildCategory::Storage && Settlement && Building->IsFinished())
		{
			Lines.Add(FString::Printf(TEXT("Lager der Siedlung: %d / %d"), Settlement->GetStoredTotal(), Settlement->GetCapacity()));
		}
		for (const AGamerVillager* Resident : GetVillagersSorted())
		{
			if (Resident->GetBed() == Building)
			{
				Lines.Add(FString::Printf(TEXT("Bett von %s"), *Resident->GetVillagerName()));
			}
		}
		if (Data)
		{
			for (const TPair<FName, FGamerJobDef>& Pair : Data->GetData().Jobs)
			{
				if (!Pair.Value.bDeprecated && Pair.Value.Station == Building->GetBuildableId())
				{
					Lines.Add(FString::Printf(TEXT("Arbeitsplatz: %s"), *Pair.Value.Name));
				}
			}
		}
	}
	else if (const AGamerResourceNode* Node = Cast<AGamerResourceNode>(Target))
	{
		const FGamerResourceNodeDef* Def = Node->GetDef();
		Lines.Add(Def ? Def->Name : Node->GetNodeId().ToString());
		Lines.Add(Node->IsDepleted() ? TEXT("abgeerntet, wächst nach") : TEXT("bereit"));
	}
	return Lines;
}

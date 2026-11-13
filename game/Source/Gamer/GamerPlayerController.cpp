#include "GamerPlayerController.h"

#include "Gamer.h"
#include "GamerCharacter.h"
#include "GamerResourceNode.h"
#include "GamerWorkComponent.h"
#include "EngineUtils.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

void AGamerPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		FParse::Value(FCommandLine::Get(), TEXT("GamerTestWalk="), PendingWalkSeconds);
		FParse::Value(FCommandLine::Get(), TEXT("GamerTestWalkDelay="), PendingWalkDelay);

		FString Script;
		if (FParse::Value(FCommandLine::Get(), TEXT("GamerTestScript="), Script, false))
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

void AGamerPlayerController::RunTestScript(float DeltaTime)
{
	AGamerCharacter* GamerCharacter = Cast<AGamerCharacter>(GetPawn());
	if (!GamerCharacter || !TestSteps.IsValidIndex(TestStepIndex))
	{
		return;
	}
	if (TestPause > 0.f)
	{
		TestPause -= DeltaTime;
		return;
	}
	UGamerWorkComponent* Work = GamerCharacter->GetWork();
	if (Work->IsBusy())
	{
		return;
	}

	const FString Step = TestSteps[TestStepIndex++];
	FString Kind, Argument;
	Step.Split(TEXT(":"), &Kind, &Argument);
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
		Work->ServerHarvest(Nearest);
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

void AGamerPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	RunTestScript(DeltaTime);

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

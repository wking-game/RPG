#include "GamerPlayerController.h"

#include "Gamer.h"
#include "GamerCharacter.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

void AGamerPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		FParse::Value(FCommandLine::Get(), TEXT("GamerTestWalk="), PendingWalkSeconds);
		FParse::Value(FCommandLine::Get(), TEXT("GamerTestWalkDelay="), PendingWalkDelay);
	}
}

void AGamerPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

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

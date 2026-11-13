#include "GamerSessionSubsystem.h"

#include "Gamer.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

void UGamerSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	if (GEngine)
	{
		NetworkFailureHandle = GEngine->OnNetworkFailure().AddUObject(this, &UGamerSessionSubsystem::HandleNetworkFailure);
		TravelFailureHandle = GEngine->OnTravelFailure().AddUObject(this, &UGamerSessionSubsystem::HandleTravelFailure);
	}
}

void UGamerSessionSubsystem::Deinitialize()
{
	if (GEngine)
	{
		GEngine->OnNetworkFailure().Remove(NetworkFailureHandle);
		GEngine->OnTravelFailure().Remove(TravelFailureHandle);
	}
	Super::Deinitialize();
}

UGamerSessionSubsystem* UGamerSessionSubsystem::Get(const UObject* WorldContext)
{
	const UWorld* World = WorldContext ? WorldContext->GetWorld() : nullptr;
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<UGamerSessionSubsystem>() : nullptr;
}

void UGamerSessionSubsystem::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	// One disconnect often raises several failures in a row; the first one says best what happened.
	const double Now = FPlatformTime::Seconds();
	if (!LastError.IsEmpty() && Now - LastErrorTime < 3.0)
	{
		UE_LOG(LogGamer, Log, TEXT("Session error (follow-up, ignored): %s %s"), ENetworkFailure::ToString(FailureType), *ErrorString);
		return;
	}
	LastErrorTime = Now;
	switch (FailureType)
	{
	case ENetworkFailure::FailureReceived:
		LastError = ErrorString.Contains(TEXT("closed")) ? FString(TEXT("Der Gastgeber hat die Welt geschlossen."))
			: TEXT("Der Gastgeber hat die Verbindung beendet: ") + ErrorString;
		break;
	case ENetworkFailure::ConnectionLost:
		LastError = TEXT("Verbindung zum Gastgeber verloren.");
		break;
	case ENetworkFailure::ConnectionTimeout:
		LastError = TEXT("Keine Antwort vom Gastgeber (Zeitüberschreitung).");
		break;
	case ENetworkFailure::PendingConnectionFailure:
		LastError = TEXT("Beitreten fehlgeschlagen: ") + ErrorString;
		break;
	default:
		LastError = FString::Printf(TEXT("Netzwerkfehler (%s): %s"), ENetworkFailure::ToString(FailureType), *ErrorString);
		break;
	}
	UE_LOG(LogGamer, Warning, TEXT("Session error: %s"), *LastError);
}

void UGamerSessionSubsystem::HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString)
{
	LastErrorTime = FPlatformTime::Seconds();
	LastError = FString::Printf(TEXT("Welt konnte nicht geladen werden (%s): %s"), ETravelFailure::ToString(FailureType), *ErrorString);
	UE_LOG(LogGamer, Warning, TEXT("Session error: %s"), *LastError);
}

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GamerSessionSubsystem.generated.h"

class UNetDriver;

/**
 * Remembers why the last connection or world change failed, so the start menu can say it
 * ("Verbindung zum Gastgeber verloren"). Lives as long as the game runs (survives map changes).
 */
UCLASS()
class GAMER_API UGamerSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	static UGamerSessionSubsystem* Get(const UObject* WorldContext);

	const FString& GetLastError() const { return LastError; }
	void ClearError() { LastError.Reset(); }

private:
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	void HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString);

	FString LastError;
	double LastErrorTime = 0.0;
	FDelegateHandle NetworkFailureHandle;
	FDelegateHandle TravelFailureHandle;
};

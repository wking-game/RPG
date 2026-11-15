#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "GamerAnimTypes.h"
#include "GamerAnimInstance.generated.h"

class UAnimSequence;
class UGamerAnimInstance;

/**
 * Pose blending in C++ (no AnimBlueprint graph, architecture rule 4):
 * idle / walk / jog / sprint by speed with a shared foot phase, jump / fall / land,
 * and a work layer (loop or one-shot) that covers the whole body when standing and only
 * the upper body (spine_01 and below it) when moving. Times advance on the game thread
 * (PreUpdate); Evaluate only samples and blends.
 */
struct FGamerAnimInstanceProxy : public FAnimInstanceProxy
{
	FGamerAnimInstanceProxy() = default;
	explicit FGamerAnimInstanceProxy(UAnimInstance* Instance) : FAnimInstanceProxy(Instance) {}

	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
	virtual bool Evaluate(FPoseContext& Output) override;

private:
	// Clips (owned by UGamerAnimInstance as UPROPERTYs).
	const UAnimSequence* Idle = nullptr;
	const UAnimSequence* Move[3] = {}; // walk, jog, sprint
	const UAnimSequence* JumpStart = nullptr;
	const UAnimSequence* FallLoop = nullptr;
	const UAnimSequence* Land = nullptr;
	const UAnimSequence* Actions[static_cast<int32>(EGamerAnimAction::Count)] = {};

	// Locomotion
	float MoveSpeeds[3] = { 170.f, 400.f, 700.f };
	int32 MoveA = 0;          // 0 = idle, 1..3 = Move[i-1]
	int32 MoveB = 0;
	float MoveBlend = 0.f;    // weight of MoveB
	float MovePhase = 0.f;    // 0..1, shared by walk/jog/sprint
	float MoveRate = 1.f;     // play rate when the speed is outside the clip range
	float IdleTime = 0.f;
	float Speed = 0.f;

	// Air
	float AirAlpha = 0.f;
	float AirTime = 0.f;
	bool bWasFalling = false;
	float LandTime = 100.f;
	float LandWeight = 0.f;

	// Work layer
	const UAnimSequence* ActionClip = nullptr;
	float ActionTime = 0.f;
	float ActionWeight = 0.f;
	bool bActionLoop = false;
	bool bOneShot = false;
	uint8 LastPulseCount = 0;
	bool bPulseSeen = false;
	float MovingAlpha = 0.f;  // 0 standing (full body), 1 moving (upper body only)
	EGamerAnimAction LastLoggedLoop = EGamerAnimAction::None;
	const UAnimSequence* LastLoggedClip = nullptr;

	void Sample(const UAnimSequence* Clip, float Time, bool bLoop, FPoseContext& Out) const;
	void UpperBodyWeights(const FBoneContainer& Bones, float Weight, float LowerWeight, TArray<float>& OutWeights) const;
};

UCLASS(Transient, NotBlueprintable)
class GAMER_API UGamerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UGamerAnimInstance();

	/** Inputs collected on the game thread in NativeUpdateAnimation; read by the proxy in PreUpdate. */
	const FGamerAnimInputs& GetInputs() const { return Inputs; }

	UPROPERTY(Transient) TObjectPtr<UAnimSequence> IdleClip;
	UPROPERTY(Transient) TObjectPtr<UAnimSequence> WalkClip;
	UPROPERTY(Transient) TObjectPtr<UAnimSequence> JogClip;
	UPROPERTY(Transient) TObjectPtr<UAnimSequence> SprintClip;
	UPROPERTY(Transient) TObjectPtr<UAnimSequence> JumpClip;
	UPROPERTY(Transient) TObjectPtr<UAnimSequence> FallClip;
	UPROPERTY(Transient) TObjectPtr<UAnimSequence> LandClip;
	UPROPERTY(Transient) TArray<TObjectPtr<UAnimSequence>> ActionClips; // by EGamerAnimAction

protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override { return new FGamerAnimInstanceProxy(this); }

private:
	FGamerAnimInputs Inputs;
};

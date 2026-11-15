#include "GamerAnimInstance.h"

#include "Animation/AnimSequence.h"
#include "Animation/AnimationPoseData.h"
#include "AnimationRuntime.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gamer.h"
#include "GamerCharacter.h"
#include "GamerVillager.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	// Blend times (seconds).
	constexpr float AirBlendTime = 0.15f;
	constexpr float ActionBlendIn = 0.12f;
	constexpr float ActionBlendOut = 0.25f;
	constexpr float MovingBlendTime = 0.2f;
	constexpr float MovingThreshold = 60.f;   // cm/s: above this, work animations play on the upper body only
	constexpr float OneShotRate = 1.25f;      // one-shot work swings a little faster than the source clips
	const FName UpperBodyRoot(TEXT("spine_01"));

	float Approach(float Current, float Target, float Step)
	{
		return Current < Target ? FMath::Min(Current + Step, Target) : FMath::Max(Current - Step, Target);
	}

	float ClipLength(const UAnimSequence* Clip)
	{
		return Clip ? FMath::Max(0.01f, Clip->GetPlayLength()) : 1.f;
	}

	template <typename T>
	T* FindAnim(const TCHAR* Path)
	{
		ConstructorHelpers::FObjectFinder<T> Finder(Path);
		return Finder.Succeeded() ? Finder.Object : nullptr;
	}
}

// ---------------------------------------------------------------------------------------------
// UGamerAnimInstance

UGamerAnimInstance::UGamerAnimInstance()
{
	// Epic UE5 Mannequin animations (engine template content) and retargeted ones (tools/import_characters.ps1).
	static UAnimSequence* Idle = FindAnim<UAnimSequence>(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/MM_Idle.MM_Idle"));
	static UAnimSequence* Walk = FindAnim<UAnimSequence>(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/Walk/MF_Unarmed_Walk_Fwd.MF_Unarmed_Walk_Fwd"));
	static UAnimSequence* Jog = FindAnim<UAnimSequence>(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/Jog/MF_Unarmed_Jog_Fwd.MF_Unarmed_Jog_Fwd"));
	static UAnimSequence* Jump = FindAnim<UAnimSequence>(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/Jump/MM_Jump.MM_Jump"));
	static UAnimSequence* Fall = FindAnim<UAnimSequence>(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/Jump/MM_Fall_Loop.MM_Fall_Loop"));
	static UAnimSequence* LandAnim = FindAnim<UAnimSequence>(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/Jump/MM_Land.MM_Land"));
	static UAnimSequence* Sprint = FindAnim<UAnimSequence>(TEXT("/Game/Gamer/Characters/Anims/A_Sprint.A_Sprint"));
	static UAnimSequence* Chop = FindAnim<UAnimSequence>(TEXT("/Game/Gamer/Characters/Anims/A_Chop.A_Chop"));
	static UAnimSequence* Mine = FindAnim<UAnimSequence>(TEXT("/Game/Gamer/Characters/Anims/A_Mine.A_Mine"));
	static UAnimSequence* Hammer = FindAnim<UAnimSequence>(TEXT("/Game/Gamer/Characters/Anims/A_Hammer.A_Hammer"));
	static UAnimSequence* Gather = FindAnim<UAnimSequence>(TEXT("/Game/Gamer/Characters/Anims/A_Gather.A_Gather"));
	static UAnimSequence* GatherLow = FindAnim<UAnimSequence>(TEXT("/Game/Gamer/Characters/Anims/A_Gather3.A_Gather3"));
	static UAnimSequence* Plant = FindAnim<UAnimSequence>(TEXT("/Game/Gamer/Characters/Anims/A_PlantSeed.A_PlantSeed"));
	static UAnimSequence* Harvest = FindAnim<UAnimSequence>(TEXT("/Game/Gamer/Characters/Anims/A_Harvest.A_Harvest"));
	static UAnimSequence* Water = FindAnim<UAnimSequence>(TEXT("/Game/Gamer/Characters/Anims/A_Water.A_Water"));
	static UAnimSequence* Consume = FindAnim<UAnimSequence>(TEXT("/Game/Gamer/Characters/Anims/A_Consume.A_Consume"));

	IdleClip = Idle;
	WalkClip = Walk;
	JogClip = Jog;
	SprintClip = Sprint ? Sprint : Jog;
	JumpClip = Jump;
	FallClip = Fall;
	LandClip = LandAnim;

	ActionClips.SetNum(static_cast<int32>(EGamerAnimAction::Count));
	ActionClips[static_cast<int32>(EGamerAnimAction::Chop)] = Chop;
	ActionClips[static_cast<int32>(EGamerAnimAction::Mine)] = Mine;
	ActionClips[static_cast<int32>(EGamerAnimAction::Hammer)] = Mine; // standing swing at a plan; A_Hammer kneels
	ActionClips[static_cast<int32>(EGamerAnimAction::Gather)] = Gather;
	ActionClips[static_cast<int32>(EGamerAnimAction::Craft)] = Hammer; // kneeling, working with the hands
	ActionClips[static_cast<int32>(EGamerAnimAction::Plant)] = Plant;
	ActionClips[static_cast<int32>(EGamerAnimAction::Harvest)] = Harvest;
	ActionClips[static_cast<int32>(EGamerAnimAction::Water)] = Water;
	ActionClips[static_cast<int32>(EGamerAnimAction::Consume)] = Consume;
	// Kevin Gathering03: crouched, the hand goes down to the ground and back (pelvis 49 cm, hand 41 cm; pose check
	// in tools/unreal/import_characters.py). Farm_Harvest only bends with a swing and looked like hacking.
	ActionClips[static_cast<int32>(EGamerAnimAction::GatherLow)] = GatherLow ? GatherLow : Harvest;
	ActionClips[static_cast<int32>(EGamerAnimAction::CraftStation)] = Gather;  // standing, hands working in front
}

void UGamerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	Inputs = FGamerAnimInputs();
	APawn* Pawn = TryGetPawnOwner();
	if (!Pawn)
	{
		return;
	}
	const FVector Velocity = Pawn->GetVelocity();
	Inputs.Speed = Velocity.Size2D();
	Inputs.VerticalSpeed = Velocity.Z;
	if (const ACharacter* Character = Cast<ACharacter>(Pawn))
	{
		Inputs.bFalling = Character->GetCharacterMovement() && Character->GetCharacterMovement()->IsFalling();
	}
	if (const AGamerCharacter* Player = Cast<AGamerCharacter>(Pawn))
	{
		Player->FillAnimInputs(Inputs);
	}
	else if (const AGamerVillager* Villager = Cast<AGamerVillager>(Pawn))
	{
		Villager->FillAnimInputs(Inputs);
	}
}

// ---------------------------------------------------------------------------------------------
// FGamerAnimInstanceProxy

void FGamerAnimInstanceProxy::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	FAnimInstanceProxy::PreUpdate(InAnimInstance, DeltaSeconds);
	const UGamerAnimInstance* Instance = CastChecked<UGamerAnimInstance>(InAnimInstance);
	const FGamerAnimInputs& In = Instance->GetInputs();
	const float Dt = FMath::Max(0.f, DeltaSeconds);

	Idle = Instance->IdleClip;
	Move[0] = Instance->WalkClip;
	Move[1] = Instance->JogClip;
	Move[2] = Instance->SprintClip;
	JumpStart = Instance->JumpClip;
	FallLoop = Instance->FallClip;
	Land = Instance->LandClip;
	for (int32 i = 0; i < UE_ARRAY_COUNT(Actions); ++i)
	{
		Actions[i] = Instance->ActionClips.IsValidIndex(i) ? Instance->ActionClips[i].Get() : nullptr;
	}

	// --- Locomotion: piecewise linear between idle (0), walk, jog and (only while sprinting) sprint.
	Speed = In.Speed;
	const int32 Top = In.bSprinting ? 3 : 2;
	float Points[4] = { 0.f, MoveSpeeds[0], MoveSpeeds[1], MoveSpeeds[2] };
	if (Speed >= Points[Top])
	{
		MoveA = MoveB = Top;
		MoveBlend = 0.f;
		MoveRate = Speed / Points[Top];
	}
	else
	{
		int32 Seg = 0;
		while (Seg + 1 < Top && Speed >= Points[Seg + 1])
		{
			++Seg;
		}
		MoveA = Seg;
		MoveB = Seg + 1;
		MoveBlend = FMath::Clamp((Speed - Points[Seg]) / FMath::Max(1.f, Points[Seg + 1] - Points[Seg]), 0.f, 1.f);
		MoveRate = 1.f;
	}
	// Shared phase: advance by distance travelled per stride of the blended moving clips.
	auto Stride = [this](int32 Index) { return Index == 0 ? 0.f : MoveSpeeds[Index - 1] * ClipLength(Move[Index - 1]); };
	float StrideLen = MoveA == 0 ? Stride(MoveB) : FMath::Lerp(Stride(MoveA), Stride(MoveB), MoveBlend);
	if (StrideLen > 1.f)
	{
		MovePhase = FMath::Fmod(MovePhase + Dt * Speed / StrideLen, 1.f);
	}
	IdleTime += Dt;

	// --- Air
	if (In.bFalling)
	{
		if (!bWasFalling)
		{
			AirTime = In.VerticalSpeed > 10.f ? 0.f : ClipLength(JumpStart); // walked off an edge: straight to the fall loop
		}
		AirTime += Dt;
	}
	else if (bWasFalling && AirTime > 0.3f)
	{
		LandTime = 0.f;
	}
	bWasFalling = In.bFalling;
	AirAlpha = Approach(AirAlpha, In.bFalling ? 1.f : 0.f, Dt / AirBlendTime);
	LandTime += Dt;
	LandWeight = Land ? FMath::Clamp(1.f - LandTime / ClipLength(Land), 0.f, 1.f) * 0.8f * (1.f - FMath::Clamp(Speed / 300.f, 0.f, 1.f)) : 0.f;

	// --- Work layer: a new pulse starts a one-shot; otherwise the loop (if any) plays.
	if (!bPulseSeen || In.Pulse.Count != LastPulseCount)
	{
		const bool bNew = bPulseSeen && In.Pulse.Kind != EGamerAnimAction::None;
		bPulseSeen = true;
		LastPulseCount = In.Pulse.Count;
		if (const UAnimSequence* Clip = bNew ? Actions[static_cast<int32>(In.Pulse.Kind)] : nullptr)
		{
			if (ActionClip != Clip || !bOneShot || ActionTime > ClipLength(Clip) * 0.5f)
			{
				ActionTime = 0.f;
			}
			ActionClip = Clip;
			bOneShot = true;
			bActionLoop = false;
		}
	}
	const UAnimSequence* LoopClip = In.Loop != EGamerAnimAction::None ? Actions[static_cast<int32>(In.Loop)] : nullptr;
	if (In.Loop != LastLoggedLoop || (bOneShot && ActionClip != LastLoggedClip))
	{
		// Which clip really plays (diagnostics for "wrong movement" reports).
		LastLoggedLoop = In.Loop;
		LastLoggedClip = bOneShot ? ActionClip : LoopClip;
		UE_LOG(LogGamer, Log, TEXT("Anim %s: loop %d, one-shot %d, clip %s"), GetSkelMeshComponent() && GetSkelMeshComponent()->GetOwner() ? *GetSkelMeshComponent()->GetOwner()->GetName() : TEXT("?"),
			static_cast<int32>(In.Loop), bOneShot ? static_cast<int32>(In.Pulse.Kind) : -1, LastLoggedClip ? *LastLoggedClip->GetName() : TEXT("none"));
	}
	float TargetWeight = 0.f;
	if (bOneShot)
	{
		ActionTime += Dt * OneShotRate;
		const float Remaining = ClipLength(ActionClip) - ActionTime;
		if (Remaining <= 0.f)
		{
			bOneShot = false;
		}
		else
		{
			TargetWeight = Remaining < ActionBlendOut ? Remaining / ActionBlendOut : 1.f;
		}
	}
	if (!bOneShot)
	{
		if (LoopClip)
		{
			if (ActionClip != LoopClip || !bActionLoop)
			{
				ActionTime = 0.f;
			}
			ActionClip = LoopClip;
			bActionLoop = true;
			ActionTime += Dt;
			TargetWeight = 1.f;
		}
		else
		{
			bActionLoop = false;
			ActionTime += Dt;
			TargetWeight = 0.f;
		}
	}
	if (bOneShot)
	{
		ActionWeight = FMath::Min(Approach(ActionWeight, 1.f, Dt / ActionBlendIn), TargetWeight);
	}
	else
	{
		ActionWeight = Approach(ActionWeight, TargetWeight, Dt / (TargetWeight > ActionWeight ? ActionBlendIn : ActionBlendOut));
	}
	MovingAlpha = Approach(MovingAlpha, Speed > MovingThreshold ? 1.f : 0.f, Dt / MovingBlendTime);
}

void FGamerAnimInstanceProxy::Sample(const UAnimSequence* Clip, float Time, bool bLoop, FPoseContext& Out) const
{
	if (!Clip)
	{
		Out.ResetToRefPose();
		return;
	}
	const float Length = ClipLength(Clip);
	const float T = bLoop ? FMath::Fmod(FMath::Max(0.f, Time), Length) : FMath::Clamp(Time, 0.f, Length);
	FAnimationPoseData Data(Out);
	Clip->GetAnimationPose(Data, FAnimExtractContext(static_cast<double>(T), false, {}, bLoop));
}

void FGamerAnimInstanceProxy::UpperBodyWeights(const FBoneContainer& Bones, float Weight, float LowerWeight, TArray<float>& OutWeights) const
{
	const int32 Num = Bones.GetCompactPoseNumBones();
	OutWeights.Init(LowerWeight, Num);
	const int32 MeshIndex = Bones.GetPoseBoneIndexForBoneName(UpperBodyRoot);
	if (MeshIndex == INDEX_NONE)
	{
		for (float& W : OutWeights)
		{
			W = Weight;
		}
		return;
	}
	const FCompactPoseBoneIndex Root = Bones.MakeCompactPoseIndex(FMeshPoseBoneIndex(MeshIndex));
	TArray<bool> Upper;
	Upper.Init(false, Num);
	// Compact poses are sorted parent before child.
	for (int32 i = 0; i < Num; ++i)
	{
		const FCompactPoseBoneIndex Index(i);
		const FCompactPoseBoneIndex Parent = Bones.GetParentBoneIndex(Index);
		Upper[i] = Index == Root || (Parent.IsValid() && Upper[Parent.GetInt()]);
		if (Upper[i])
		{
			OutWeights[i] = Weight;
		}
	}
}

bool FGamerAnimInstanceProxy::Evaluate(FPoseContext& Output)
{
	// Locomotion
	auto SampleMove = [this](int32 Index, FPoseContext& Out)
	{
		if (Index == 0)
		{
			Sample(Idle, IdleTime, true, Out);
		}
		else
		{
			const UAnimSequence* Clip = Move[Index - 1];
			Sample(Clip, MovePhase * ClipLength(Clip), true, Out);
		}
	};
	SampleMove(MoveA, Output);
	if (MoveB != MoveA && MoveBlend > 0.f)
	{
		FPoseContext Other(Output);
		SampleMove(MoveB, Other);
		FAnimationPoseData OutData(Output);
		FAnimationRuntime::BlendTwoPosesTogetherInPlace(OutData, FAnimationPoseData(Other), 1.f - MoveBlend);
	}

	// Landing (full body, fades out)
	if (LandWeight > ZERO_ANIMWEIGHT_THRESH)
	{
		FPoseContext LandPose(Output);
		Sample(Land, LandTime, false, LandPose);
		FAnimationPoseData OutData(Output);
		FAnimationRuntime::BlendTwoPosesTogetherInPlace(OutData, FAnimationPoseData(LandPose), 1.f - LandWeight);
	}

	// Air
	if (AirAlpha > ZERO_ANIMWEIGHT_THRESH)
	{
		FPoseContext AirPose(Output);
		const float JumpLen = ClipLength(JumpStart);
		if (JumpStart && AirTime < JumpLen)
		{
			Sample(JumpStart, AirTime, false, AirPose);
		}
		else
		{
			Sample(FallLoop, AirTime - JumpLen, true, AirPose);
		}
		FAnimationPoseData OutData(Output);
		FAnimationRuntime::BlendTwoPosesTogetherInPlace(OutData, FAnimationPoseData(AirPose), 1.f - AirAlpha);
	}

	// Work layer: whole body when standing, upper body when moving; never in the air.
	const float Weight = ActionWeight * (1.f - AirAlpha);
	if (ActionClip && Weight > ZERO_ANIMWEIGHT_THRESH)
	{
		FPoseContext ActionPose(Output);
		Sample(ActionClip, ActionTime, bActionLoop, ActionPose);
		TArray<float> Weights;
		UpperBodyWeights(Output.Pose.GetBoneContainer(), Weight, Weight * (1.f - MovingAlpha), Weights);
		FPoseContext Base(Output);
		Base.Pose.CopyBonesFrom(Output.Pose);
		Base.Curve.CopyFrom(Output.Curve);
		Base.CustomAttributes.CopyFrom(Output.CustomAttributes);
		FAnimationPoseData OutData(Output);
		FAnimationRuntime::BlendTwoPosesTogetherPerBone(FAnimationPoseData(Base), FAnimationPoseData(ActionPose), Weights, OutData);
	}
	return true;
}

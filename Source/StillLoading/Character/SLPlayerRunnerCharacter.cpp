#include "Character/SLPlayerRunnerCharacter.h"
#include "Character/Animation/SLRunnerAnimInstance.h"
#include "Minigame/Object/SLRunnerHurdle.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "InputActionValue.h"
#include "Math/RotationMatrix.h"
#include "TimerManager.h"
#include "Minigame\System/SLSplineTrack.h"

ASLPlayerRunnerCharacter::ASLPlayerRunnerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(GetRootComponent());
	BoxComp->InitBoxExtent(FVector(50.f));
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComp->SetGenerateOverlapEvents(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->TargetArmLength = Cam_Default.TargetArmLength;
	SpringArm->SetRelativeRotation(Cam_Default.ArmRotation);
	SpringArm->SocketOffset = Cam_Default.SocketOffset;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 10.f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->FieldOfView = Cam_Default.FOV;

	DynamicIMCComponent = CreateDefaultSubobject<UDynamicIMCComponent>(TEXT("DynamicIMCComponent"));

	auto* Move = GetCharacterMovement();
	Move->bOrientRotationToMovement = false;
	Move->bUseControllerDesiredRotation = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	CurrentHealth = MaxHealth;

	Cam_Default = FRunnerCamPreset();
	Cam_Slide = FRunnerCamPreset{ 320.f, FVector(0.f, 0.f, 40.f), FRotator(-5.f, 0.f, 0.f), 88.f, 10.f, 0.2f, 10.f, 0.f };
	Cam_Jump  = FRunnerCamPreset{ 380.f, FVector(0.f, 60.f, 90.f), FRotator(-12.f, 0.f, 0.f), 92.f, 8.f, 0.15f, 8.f, 90.f };
	Cam_Attack= FRunnerCamPreset{ 330.f, FVector(0.f, 20.f, 80.f), FRotator(-8.f, 0.f, 0.f), 87.f, 12.f, 0.1f, 12.f, 0.f };
	Cam_Hit   = FRunnerCamPreset{ 300.f, FVector(0.f, -10.f, 70.f), FRotator(-6.f, 0.f, 0.f), 85.f, 14.f, 0.3f, 12.f, -20.f };

	CamTarget = Cam_Default;
	CamCurrent = Cam_Default;
}


void ASLPlayerRunnerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (DynamicIMCComponent)
	{
		DynamicIMCComponent->OnActionTriggered.AddDynamic(this, &ASLPlayerRunnerCharacter::OnActionTriggeredCallback);
		DynamicIMCComponent->OnActionStarted.AddDynamic(this, &ASLPlayerRunnerCharacter::OnActionStartedCallback);
		DynamicIMCComponent->OnActionCompleted.AddDynamic(this, &ASLPlayerRunnerCharacter::OnActionCompletedCallback);
	}
	if (BoxComp)
	{
		BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ASLPlayerRunnerCharacter::OnOverlapBegin);
	}
	if (UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
	{
		Anim->OnMontageEnded.AddDynamic(this, &ASLPlayerRunnerCharacter::OnMontageEnded);
		Anim->OnMontageBlendingOut.AddDynamic(this, &ASLPlayerRunnerCharacter::OnMontageBlendingOutStarted);
	}
	
	if (!CurrentTrack)
	{
		UE_LOG(LogTemp, Error, TEXT("[Runner] TrackSplineComponent is not set or not a SplineComponent."));
		return;
	}

	TrackSpline = CurrentTrack->GetSplineComp();
	
	{
		PointDistances.Reset();
		const int32 Pts = TrackSpline->GetNumberOfSplinePoints();
		LastPointIndex = FMath::Max(0, Pts - 1);
		for (int32 i = 0; i < Pts; ++i)
		{
			PointDistances.Add(TrackSpline->GetDistanceAlongSplineAtSplinePoint(i));
		}
		GoalDistance = (PointDistances.Num() > 0) ? PointDistances.Last() : 0.f;
		bGoalReached = false;

		if (bStartAtSplineStart)
		{
			CurrentPointIndex = 0;
			SplineDistance = 0.f;
			CurrentSegmentEndDistance = (PointDistances.Num() >= 2) ? PointDistances[1] : 0.f;
		}
		else
		{
			SnapToNearestOnSpline();
			MapDistanceToSegment(SplineDistance);
		}
		ApplyTransformAtDistance(SplineDistance);
	}

	SetCameraPreset(ECameraPreset::Default, 0.f);
}

void ASLPlayerRunnerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (TrackSpline && bSplineDriveEnabled && !bGoalReached)
	{
		AdvanceAlongSegment(DeltaSeconds);
		ApplyTransformAtDistance(SplineDistance);
	}

	if (TrackSpline && !bSplineDriveEnabled && !bGoalReached)
	{
		ApplyRotationAtDistance(SplineDistance);
	}

	LateralOffset += CurrentLateralInput * LateralSpeed * DeltaSeconds;
	LateralOffset  = FMath::Clamp(LateralOffset, -LateralClamp, LateralClamp);

	ApplyCamera(DeltaSeconds);
}

void ASLPlayerRunnerCharacter::BuildPointDistances()
{
	PointDistances.Reset();
	if (!TrackSpline) return;

	const int32 Pts = TrackSpline->GetNumberOfSplinePoints();
	LastPointIndex = FMath::Max(0, Pts - 1);
	for (int32 i = 0; i < Pts; ++i)
	{
		PointDistances.Add(TrackSpline->GetDistanceAlongSplineAtSplinePoint(i));
	}
	GoalDistance = (PointDistances.Num() > 0) ? PointDistances.Last() : 0.f;
}

void ASLPlayerRunnerCharacter::MapDistanceToSegment(float Distance)
{
	if (!TrackSpline || PointDistances.Num() == 0) return;

	if (Distance >= GoalDistance - KINDA_SMALL_NUMBER)
	{
		SplineDistance = GoalDistance;
		CurrentPointIndex = LastPointIndex;
		CurrentSegmentEndDistance = GoalDistance;
		ReachGoal();
		return;
	}

	for (int32 i = 0; i < LastPointIndex; ++i)
	{
		const float S = PointDistances[i];
		const float E = PointDistances[i + 1];
		if (Distance >= S - KINDA_SMALL_NUMBER && Distance < E - KINDA_SMALL_NUMBER)
		{
			CurrentPointIndex = i;
			SplineDistance = Distance;
			CurrentSegmentEndDistance = E;
			return;
		}
	}

	CurrentPointIndex = 0;
	SplineDistance = 0.f;
	CurrentSegmentEndDistance = (PointDistances.Num() >= 2) ? PointDistances[1] : 0.f;
}

void ASLPlayerRunnerCharacter::AdvanceAlongSegment(float DeltaSeconds)
{
	if (CurrentPointIndex >= LastPointIndex)
	{
		ReachGoal();
		return;
	}

	float Advance = ForwardSpeed * DeltaSeconds;

	while (Advance > 0.f && !bGoalReached)
	{
		const float Rem = CurrentSegmentEndDistance - SplineDistance;
		if (Advance >= Rem - KINDA_SMALL_NUMBER)
		{
			SplineDistance = CurrentSegmentEndDistance;
			Advance -= Rem;

			if (CurrentPointIndex + 1 >= LastPointIndex)
			{
				ReachGoal();
				break;
			}
			else
			{
				++CurrentPointIndex;
				CurrentSegmentEndDistance = PointDistances[CurrentPointIndex + 1];
			}
		}
		else
		{
			SplineDistance += Advance;
			Advance = 0.f;
		}
	}
}

void ASLPlayerRunnerCharacter::ReachGoal()
{
	if (CurrentTrack->GetNextTrack() != nullptr)
	{
		SwitchToTrack(CurrentTrack->GetNextTrack());
		return;
	}

	if (bGoalReached) return;
	bGoalReached = true;
	bSplineDriveEnabled = false;
	SplineDistance = GoalDistance;
	ApplyTransformAtDistance(SplineDistance);
	SetCameraPreset(ECameraPreset::Default, 0.f);
}

void ASLPlayerRunnerCharacter::SnapToNearestOnSpline()
{
	if (!TrackSpline) return;
	const float Key = TrackSpline->FindInputKeyClosestToWorldLocation(GetActorLocation());
	SplineDistance = TrackSpline->GetDistanceAlongSplineAtSplineInputKey(Key);
}

void ASLPlayerRunnerCharacter::SetSplineDistance(const float NewDistance)
{
	if (!TrackSpline) return;
	const float D = FMath::Clamp(NewDistance, 0.f, GoalDistance);
	MapDistanceToSegment(D);
	ApplyTransformAtDistance(SplineDistance);
}

void ASLPlayerRunnerCharacter::SetSplineDriveEnabled(const bool bEnable)
{
	bSplineDriveEnabled = bEnable && !bGoalReached;
}

void ASLPlayerRunnerCharacter::OnActionTriggeredCallback(const EInputActionType ActionType, const FInputActionValue InputValue)
{
	switch (ActionType)
	{
	case EInputActionType::EIAT_MoveLeft:
		CurrentLateralInput = -InputValue.Get<float>();
		break;
	case EInputActionType::EIAT_MoveRight:
		CurrentLateralInput =  InputValue.Get<float>();
		break;
	default:
		break;
	}
}

void ASLPlayerRunnerCharacter::OnActionCompletedCallback(EInputActionType ActionType)
{
	switch (ActionType)
	{
	case EInputActionType::EIAT_MoveLeft:
	case EInputActionType::EIAT_MoveRight:
		CurrentLateralInput = 0.f;
		break;
	default:
		break;
	}
}

void ASLPlayerRunnerCharacter::OnActionStartedCallback(EInputActionType ActionType)
{
	switch (ActionType)
	{
	case EInputActionType::EIAT_Jump:
		CurrentState = EHurdleState::Jump;
		GetWorldTimerManager().SetTimer(StateTimerHandle, [this]{ CurrentState = EHurdleState::None; }, 0.5f, false);
		break;
	case EInputActionType::EIAT_Special:
		CurrentState = EHurdleState::Sliding;
		GetWorldTimerManager().SetTimer(StateTimerHandle, [this]{ CurrentState = EHurdleState::None; }, 0.5f, false);
		break;
	case EInputActionType::EIAT_Attack:
		CurrentState = EHurdleState::Attack;
		GetWorldTimerManager().SetTimer(StateTimerHandle, [this]{ CurrentState = EHurdleState::None; }, 0.5f, false);
		break;
	default:
		break;
	}
}

void ASLPlayerRunnerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bInvincible) return;
	if (!OtherActor || !OtherActor->ActorHasTag(TEXT("Hurdle"))) return;

	if (ASLRunnerHurdle* Hurdle = Cast<ASLRunnerHurdle>(OtherActor))
	{
		const EHurdleState ObState = Hurdle->GetHurdleState();
		const ERunnerMontageSection Section = Hurdle->GetRunnerMontageSection();
		float Duration = 0.5f;
		if (ObState == CurrentState && CurrentState != EHurdleState::None)
		{
			if (USLRunnerAnimInstance* Anim = GetRunnerAnim())
			{
				Anim->PlayMatchedMontage(ObState, Section);

				if (ObState == EHurdleState::Jump || ObState == EHurdleState::Sliding)
				{
					if (ObState == EHurdleState::Jump   && Anim->JumpMontage)  Duration = Anim->JumpMontage->GetPlayLength();
					if (ObState == EHurdleState::Sliding&& Anim->SlideMontage) Duration = Anim->SlideMontage->GetPlayLength();
					EnterRootMotionAction(Duration);

					if (ObState == EHurdleState::Sliding)
					{
						SetCameraPreset(ECameraPreset::Slide, Duration);
						StartShake(SlideShakeClass, 1.f);
					}
					else
					{
						SetCameraPreset(ECameraPreset::Jump, Duration);
						StartShake(JumpShakeClass, 1.f);
					}
				}
				else if (ObState == EHurdleState::Attack)
				{
					SetCameraPreset(ECameraPreset::Attack, 0.35f);
					StartShake(AttackShakeClass, 0.8f);
				}
			}
			StartIFrame(IFrameDuration);
		}
		else
		{
			if (USLRunnerAnimInstance* Anim = GetRunnerAnim())
			{
				Anim->PlayHitMontage(ObState, Section);

				if (ObState == EHurdleState::Jump    && Anim->HitByJumpObstacleMontage)
				{
					Duration = Anim->HitByJumpObstacleMontage->GetPlayLength();
				}
				else if (ObState == EHurdleState::Sliding && Anim->HitBySlidingObstacleMontage)
				{
					Duration = Anim->HitBySlidingObstacleMontage->GetPlayLength();
				} 
				else if (ObState == EHurdleState::Attack  && Anim->HitByAttackObstacleMontage)
				{
					Duration = Anim->HitByAttackObstacleMontage->GetPlayLength();
				}
				EnterRootMotionAction(Duration);

				SetCameraPreset(ECameraPreset::Hit, Duration);
				StartShake(HitShakeClass, 1.f);
			}
			ApplyDamage();
			StartIFrame(IFrameDuration);
		}
	}
}

void ASLPlayerRunnerCharacter::OnDie()
{
	
}

USLRunnerAnimInstance* ASLPlayerRunnerCharacter::GetRunnerAnim() const
{
	return GetMesh() ? Cast<USLRunnerAnimInstance>(GetMesh()->GetAnimInstance()) : nullptr;
}

void ASLPlayerRunnerCharacter::PushToAnim(ERunnerAction Action) const
{
	if (USLRunnerAnimInstance* Anim = GetRunnerAnim())
	{
		Anim->PushAction(Action);
	}
}

void ASLPlayerRunnerCharacter::StartIFrame(float Duration)
{
	bInvincible = true;
	GetWorldTimerManager().ClearTimer(IFrameTimerHandle);
	GetWorldTimerManager().SetTimer(IFrameTimerHandle, [this]{ bInvincible = false; }, Duration, false);
}

void ASLPlayerRunnerCharacter::EnterRootMotionAction(float ExpectedDuration)
{
	if (TrackSpline)
	{
		bSplineDriveEnabled = false;
		SplineDistOnActionStart = SplineDistance;
	}

	SetAnimRootMotionTranslationScale(1.f);

	auto* Move = GetCharacterMovement();
	Move->StopMovementImmediately();
	Move->SetMovementMode(MOVE_Flying);
	Move->GravityScale = 0.f;

	GetWorldTimerManager().SetTimer(
		ActionRootMotionTimer, this, &ASLPlayerRunnerCharacter::ExitRootMotionAction,
		ExpectedDuration, false);
}

void ASLPlayerRunnerCharacter::ExitRootMotionAction()
{
	auto* Move = GetCharacterMovement();
	Move->GravityScale = 1.f;
	Move->SetMovementMode(MOVE_Walking);

	if (TrackSpline)
	{
		const float Key = TrackSpline->FindInputKeyClosestToWorldLocation(GetActorLocation());
		float Dist = TrackSpline->GetDistanceAlongSplineAtSplineInputKey(Key);
		MapDistanceToSegment(Dist);
		bSplineDriveEnabled = !bGoalReached;
	}

	CurrentState = EHurdleState::None;
	GetWorldTimerManager().ClearTimer(ActionRootMotionTimer);
}

void ASLPlayerRunnerCharacter::ApplyTransformAtDistance(float Distance)
{
	if (!TrackSpline) return;

	const FRotator Rot = TrackSpline->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
	FRotator UseRot = Rot;
	if (!bFollowFullRotation) UseRot = FRotator(0.f, Rot.Yaw, 0.f);
	if (bSmoothRotation)
	{
		const FRotator Cur = GetActorRotation();
		UseRot = FMath::RInterpTo(Cur, UseRot, GetWorld()->GetDeltaSeconds(), RotationInterpSpeed);
	}

	const FVector BaseLoc = TrackSpline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
	const FVector Right = FRotationMatrix(UseRot).GetUnitAxis(EAxis::Y);
	const FVector FinalLoc = BaseLoc + Right * LateralOffset;

	SetActorLocationAndRotation(FinalLoc, UseRot, false, nullptr, ETeleportType::TeleportPhysics);
}

void ASLPlayerRunnerCharacter::ApplyRotationAtDistance(float Distance)
{
	if (!TrackSpline) return;

	const FRotator Rot = TrackSpline->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
	FRotator UseRot = Rot;
	if (!bFollowFullRotation) UseRot = FRotator(0.f, Rot.Yaw, 0.f);
	if (bSmoothRotation)
	{
		const FRotator Cur = GetActorRotation();
		UseRot = FMath::RInterpTo(Cur, UseRot, GetWorld()->GetDeltaSeconds(), RotationInterpSpeed);
	}
	SetActorRotation(UseRot, ETeleportType::TeleportPhysics);
}


void ASLPlayerRunnerCharacter::SwitchToTrack(ASLSplineTrack* NewTrack)
{
	checkf(NewTrack, TEXT("[Runner] SwitchToTrack: NewTrack is nullptr"));

	CurrentTrack  = NewTrack;
	TrackSpline = NewTrack->GetSplineComp();
	bGoalReached  = false;
	bSplineDriveEnabled = true;

	BuildPointDistances();

	CurrentPointIndex = 0;
	SplineDistance = 0.f;
	CurrentSegmentEndDistance = (PointDistances.Num() >= 2) ? PointDistances[1] : 0.f;

	ApplyTransformAtDistance(SplineDistance);

	// SetCameraPreset(ECameraPreset::Default, 0.15f);
}

void ASLPlayerRunnerCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!Montage) return;

	if (!bSplineDriveEnabled)
	{
		ExitRootMotionAction();
	}
	SetCameraPreset(ECameraPreset::Default, 0.25f);
}

void ASLPlayerRunnerCharacter::OnMontageBlendingOutStarted(UAnimMontage* Montage, bool bInterrupted)
{
	if (!IsHitMontage(Montage)) return;
	if (USLRunnerAnimInstance* A = GetRunnerAnim())
	{
		A->PlayRunStart();
	}
}

bool ASLPlayerRunnerCharacter::IsHitMontage(const UAnimMontage* Montage) const
{
	if (!Montage) return false;
	if (const USLRunnerAnimInstance* Anim = GetRunnerAnim())
	{
		return Montage == Anim->HitByJumpObstacleMontage
			|| Montage == Anim->HitBySlidingObstacleMontage
			|| Montage == Anim->HitByAttackObstacleMontage;
	}
	return false;
}

void ASLPlayerRunnerCharacter::SetCameraPreset(ECameraPreset Preset, float HoldTime)
{
	ActivePreset = Preset;

	switch (Preset)
	{
	case ECameraPreset::Default: CamTarget = Cam_Default; break;
	case ECameraPreset::Slide:   CamTarget = Cam_Slide;   break;
	case ECameraPreset::Jump:    CamTarget = Cam_Jump;    break;
	case ECameraPreset::Attack:  CamTarget = Cam_Attack;  break;
	case ECameraPreset::Hit:     CamTarget = Cam_Hit;     break;
	default: CamTarget = Cam_Default; break;
	}

	GetWorldTimerManager().ClearTimer(CamPresetTimer);
	if (HoldTime > 0.f)
	{
		GetWorldTimerManager().SetTimer(CamPresetTimer, [this]()
		{
			ActivePreset = ECameraPreset::Default;
			CamTarget = Cam_Default;
		}, HoldTime, false);
	}
}

void ASLPlayerRunnerCharacter::ApplyCamera(float DeltaSeconds)
{
	CamCurrent.TargetArmLength = FMath::FInterpTo(CamCurrent.TargetArmLength, CamTarget.TargetArmLength, DeltaSeconds, CamTarget.BlendSpeed);
	CamCurrent.SocketOffset    = FMath::VInterpTo(CamCurrent.SocketOffset,    CamTarget.SocketOffset,    DeltaSeconds, CamTarget.BlendSpeed);
	CamCurrent.ArmRotation     = FMath::RInterpTo(CamCurrent.ArmRotation,     CamTarget.ArmRotation,     DeltaSeconds, CamTarget.BlendSpeed);
	CamCurrent.FOV             = FMath::FInterpTo(CamCurrent.FOV,             CamTarget.FOV,             DeltaSeconds, CamTarget.BlendSpeed);
	CamCurrent.ShakeAmplitude  = FMath::FInterpTo(CamCurrent.ShakeAmplitude,  CamTarget.ShakeAmplitude,  DeltaSeconds, CamTarget.BlendSpeed);
	CamCurrent.ShakeFrequency  = FMath::FInterpTo(CamCurrent.ShakeFrequency,  CamTarget.ShakeFrequency,  DeltaSeconds, CamTarget.BlendSpeed);
	CamCurrent.YawAroundCharacter = FMath::FInterpTo(CamCurrent.YawAroundCharacter, CamTarget.YawAroundCharacter, DeltaSeconds, CamTarget.BlendSpeed);

	SpringArm->TargetArmLength = CamCurrent.TargetArmLength;
	FollowCamera->SetFieldOfView(CamCurrent.FOV);

	FRotator FinalRot = CamCurrent.ArmRotation;
	FinalRot.Yaw += CamCurrent.YawAroundCharacter;
	SpringArm->SetRelativeRotation(FinalRot);

	CamNoiseTime += DeltaSeconds * FMath::Max(0.01f, CamCurrent.ShakeFrequency);
	const float NX = FMath::PerlinNoise1D(CamNoiseTime);
	const float NY = FMath::PerlinNoise1D(CamNoiseTime + 37.123f);
	const FVector Noise = FVector(NX * CamCurrent.ShakeAmplitude * 2.f, NY * CamCurrent.ShakeAmplitude * 2.f, NX * CamCurrent.ShakeAmplitude * 0.8f);

	SpringArm->SocketOffset = CamCurrent.SocketOffset + Noise;
}

void ASLPlayerRunnerCharacter::StartShake(TSubclassOf<UCameraShakeBase> ShakeClass, float Scale)
{
	if (!ShakeClass) return;
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->StartCameraShake(ShakeClass, Scale);
		}
	}
}
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
#include "Minigame/System/SLSplineTrack.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Objective/SLObjectiveBase.h"
#include "Objective/SLObjectiveSubsystem.h"
#include "UI/HUD/SLInGameHUD.h"


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
	Cam_Slide   = FRunnerCamPreset{ 320.f, FVector(0.f, 0.f, 40.f), FRotator(-5.f, 0.f, 0.f), 88.f, 10.f, 0.2f, 10.f, 0.f };
	Cam_Jump    = FRunnerCamPreset{ 380.f, FVector(0.f, 60.f, 90.f), FRotator(-12.f, 0.f, 0.f), 92.f, 8.f,  0.15f, 8.f,  90.f };
	Cam_Attack  = FRunnerCamPreset{ 330.f, FVector(0.f, 20.f, 80.f), FRotator(-8.f,  0.f, 0.f), 87.f, 12.f, 0.1f,  12.f, 0.f };
	Cam_Hit     = FRunnerCamPreset{ 300.f, FVector(0.f,-10.f, 70.f), FRotator(-6.f,  0.f, 0.f), 85.f, 14.f, 0.3f,  12.f,-20.f };
	Cam_Death   = FRunnerCamPreset{ 260.f, FVector(0.f,-20.f, 60.f), FRotator(-10.f, 0.f, 0.f), 84.f, 10.f, 0.25f, 10.f, -10.f };

	CamTarget  = Cam_Default;
	CamCurrent = Cam_Default;
}

void ASLPlayerRunnerCharacter::BeginPlay()
{
	Super::BeginPlay();

	RealPrev = FPlatformTime::Seconds();

	if (DynamicIMCComponent)
	{
		DynamicIMCComponent->OnActionTriggered.AddDynamic(this, &ASLPlayerRunnerCharacter::OnActionTriggeredCallback);
	}
	if (UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
	{
		Anim->OnMontageEnded.AddDynamic(this, &ASLPlayerRunnerCharacter::OnMontageEnded);
		Anim->OnMontageBlendingOut.AddDynamic(this, &ASLPlayerRunnerCharacter::OnMontageBlendingOutStarted);
	}
	if (BoxComp)
	{
		BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ASLPlayerRunnerCharacter::OnOverlapedHurdle);
	}
	if (!CurrentTrack)
	{
		UE_LOG(LogTemp, Error, TEXT("[Runner] Start Track is not set"));
		return;
	}

	TrackSpline = CurrentTrack->GetSplineComp();
	BuildPointDistances();

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
	SetCameraPreset(ECameraPreset::Default, 0.f);

	if (USkeletalMeshComponent* M = GetMesh())
	{
		MeshDefaultRelative = M->GetRelativeTransform();
	}
	if (SpringArm)
	{
		SpringArmDefaultRelative = SpringArm->GetRelativeTransform();
	}
	if (FollowCamera)
	{
		CameraDefaultRelative = FollowCamera->GetRelativeTransform();
	}
	
	if (EntrySequenceActor)
	{
		bGameStarted        = false;
		bSplineDriveEnabled = false;
		bPlayingSequence    = true;

		if (UCharacterMovementComponent* Move = GetCharacterMovement())
		{
			Move->StopMovementImmediately();
			Move->SetMovementMode(MOVE_None);
		}
		if (USLRunnerAnimInstance* RunAnim = GetRunnerAnim())
		{
			RunAnim->bIsPlayingSequence = true;
		}

		PlayEntrySequence();
	}
	else
	{
		bGameStarted        = true;
		bSplineDriveEnabled = true;
	}
}

void ASLPlayerRunnerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const double Now = FPlatformTime::Seconds();
	const float RealDelta = static_cast<float>(Now - RealPrev);
	RealPrev = Now;

	if (bIsDead)
	{
		ApplyCamera(DeltaSeconds);
		return;
	}

	if (bQTEActive)
	{
		QTEElapsedSeconds += RealDelta;
		const float PressNorm = (QTEWindowSeconds > KINDA_SMALL_NUMBER) ? (QTEElapsedSeconds / QTEWindowSeconds) : 0.f;
		if (PressNorm >= QTENormalizedLateGrace)
		{
			PrintResult(EQTERating::Miss);
			bIsSuccess     = false;
			bWaitingResult = true;
			QTE_End();
		}
	}

	if (bMergingToTrack)
	{
		TickMergeToTrack(DeltaSeconds);
		ApplyCamera(DeltaSeconds);
		return;
	}

	if (bTrackBlendActive)
	{
		TrackBlendAlpha = FMath::Clamp(
			TrackBlendAlpha + (TrackBlendDuration > KINDA_SMALL_NUMBER ? DeltaSeconds / TrackBlendDuration : 1.f),
			0.f, 1.f);

		const FVector L = FMath::Lerp(TrackBlendStart.GetLocation(), TrackBlendTarget.GetLocation(), TrackBlendAlpha);
		const FQuat   R = FQuat::Slerp(TrackBlendStart.GetRotation(), TrackBlendTarget.GetRotation(), TrackBlendAlpha);
		SetActorLocationAndRotation(L, R.Rotator(), false, nullptr, ETeleportType::TeleportPhysics);

		if (FMath::IsNearlyEqual(TrackBlendAlpha, 1.f))
		{
			bTrackBlendActive = false;

			if (PendingTrack)
			{
				if (PendingSpline)
				{
					const float NewKey = PendingSpline->FindInputKeyClosestToWorldLocation(MergeStartLoc);
					PendingSplineDistance = FMath::Clamp(
						PendingSpline->GetDistanceAlongSplineAtSplineInputKey(NewKey) + 120.f,
						0.f, PendingSpline->GetSplineLength());
				}
				SwitchToTrackAtDistance(PendingTrack, PendingSplineDistance);
				PendingTrack  = nullptr;
				PendingSpline = nullptr;
			}
		}

		ApplyCamera(DeltaSeconds);
		return;
	}

	if (bPlayingSequence)
	{
		LastSequenceWorldTransform = GetActorTransform();
		bLastSeqTransformValid = true;

		ApplyCamera(DeltaSeconds);
		return;
	}

	if (USLRunnerAnimInstance* Anim = GetRunnerAnim())
	{
		const bool bFullBodyActive =
			(Anim->JumpMontage                 && Anim->Montage_IsPlaying(Anim->JumpMontage)) ||
			(Anim->SlideMontage                && Anim->Montage_IsPlaying(Anim->SlideMontage)) ||
			(Anim->HitByJumpObstacleMontage    && Anim->Montage_IsPlaying(Anim->HitByJumpObstacleMontage)) ||
			(Anim->HitBySlidingObstacleMontage && Anim->Montage_IsPlaying(Anim->HitBySlidingObstacleMontage));
		if (bFullBodyActive)
		{
			bSplineDriveEnabled = false;
		}
	}

	if (TrackSpline && bSplineDriveEnabled && !bGoalReached && bGameStarted)
	{
		AdvanceAlongSegment(DeltaSeconds);
		ApplyTransformAtDistance(SplineDistance);
	}
	else if (TrackSpline && !bSplineDriveEnabled && !bGoalReached)
	{
		ApplyRotationAtDistance(SplineDistance, /*bForceSnap=*/false);
	}

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

	while (Advance > 0.f && !bGoalReached && bSplineDriveEnabled)
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

	constexpr float HardReachTolerance = 5.f;
	if (!bGoalReached && (GoalDistance - SplineDistance) <= HardReachTolerance)
	{
		SplineDistance = GoalDistance;
		ReachGoal();
	}
}

void ASLPlayerRunnerCharacter::ReachGoal()
{
	if (CurrentTrack && CurrentTrack->GetNextTrack() != nullptr)
	{
		bSplineDriveEnabled = false;
		bGoalReached        = true;

		if (ALevelSequenceActor* SeqActor = CurrentTrack->GetLevelSequenceActor())
		{
			PlayTransitionTrackSequence(SeqActor);
		}
		else
		{
			StartMergeToTrack(CurrentTrack->GetNextTrack());
		}
		return;
	}

	if (bGoalReached) return;
	OnPlayerSusccess.Broadcast();
	bGoalReached = true;
	bSplineDriveEnabled = false;
	SplineDistance = GoalDistance;
	ApplyTransformAtDistance(SplineDistance, /*bForceSnap=*/true);
	SetCameraPreset(ECameraPreset::Default, 0.f);
}

void ASLPlayerRunnerCharacter::SnapToNearestOnSpline()
{
	if (!TrackSpline) return;
	const float Key = TrackSpline->FindInputKeyClosestToWorldLocation(GetActorLocation());
	SplineDistance = TrackSpline->GetDistanceAlongSplineAtSplineInputKey(Key);
}

void ASLPlayerRunnerCharacter::HurdleSuccess(EHurdleState ObState, ERunnerMontageSection Section)
{
	float Duration = 0.5f;
	if (USLRunnerAnimInstance* Anim = GetRunnerAnim())
	{
		Anim->PlayMatchedMontage(ObState, Section);

		if (ObState == EHurdleState::Jump || ObState == EHurdleState::Sliding)
		{
			if (ObState == EHurdleState::Jump    && Anim->JumpMontage)  Duration = Anim->JumpMontage->GetPlayLength();
			if (ObState == EHurdleState::Sliding && Anim->SlideMontage) Duration = Anim->SlideMontage->GetPlayLength();
			EnterRootMotionAction(Duration);

			if (ObState == EHurdleState::Sliding) { SetCameraPreset(ECameraPreset::Slide, Duration);  StartShake(SlideShakeClass,  1.f); }
			else                                  { SetCameraPreset(ECameraPreset::Jump,  Duration);  StartShake(JumpShakeClass,   1.f); }
		}
		else if (ObState == EHurdleState::Attack)
		{
			SetCameraPreset(ECameraPreset::Attack, 0.35f);
			StartShake(AttackShakeClass, 0.8f);
		}
	}
	StartIFrame(IFrameDuration);
}

void ASLPlayerRunnerCharacter::HurdleFail(EHurdleState ObState, ERunnerMontageSection Section)
{
	float Duration = 0.5f;
	if (USLRunnerAnimInstance* Anim = GetRunnerAnim())
	{
		Anim->PlayHitMontage(ObState, Section);
		if (ObState == EHurdleState::Jump || ObState == EHurdleState::Sliding)
		{
			if (ObState == EHurdleState::Jump    && Anim->HitByJumpObstacleMontage)        Duration = Anim->HitByJumpObstacleMontage->GetPlayLength();
			else if (ObState == EHurdleState::Sliding && Anim->HitBySlidingObstacleMontage) Duration = Anim->HitBySlidingObstacleMontage->GetPlayLength();

			EnterRootMotionAction(Duration);
			SetCameraPreset(ECameraPreset::Hit, Duration);
			StartShake(HitShakeClass, 1.f);
		}
		else if (ObState == EHurdleState::Attack)
		{
			SetCameraPreset(ECameraPreset::Attack, 0.35f);
			StartShake(AttackShakeClass, 0.8f);
		}
	}
	OnHitVFX();
	ApplyDamage();
	StartIFrame(IFrameDuration);
}

void ASLPlayerRunnerCharacter::QTE_Begin(const FString& ExpectedKey, float WindowSeconds, EHurdleState State, ERunnerMontageSection SuccessSection, ERunnerMontageSection FailSection)
{
	bQTEActive = true;
	bInputLatched = false;
	QTEExpectedKey   = ExpectedKey;
	QTEWindowSeconds = FMath::Max(0.f, WindowSeconds);
	QTEElapsedSeconds= 0.f;
	InputKey         = "";
	InputTime        = 0.f;
	QTEState         = State;
	QTESuccessSection= SuccessSection;
	QTEFailSection   = FailSection;
}

void ASLPlayerRunnerCharacter::QTE_End()
{
	bQTEActive = false;
	bInputLatched = false;
	QTEExpectedKey = "";
	QTEWindowSeconds = 0.f;
	QTEElapsedSeconds = 0.f;
	InputKey = "";
	InputTime = 0.f;
	QTEState = EHurdleState::None;
	QTESuccessSection = ERunnerMontageSection::None;
	QTEFailSection    = ERunnerMontageSection::None;
}

void ASLPlayerRunnerCharacter::OnActionTriggeredCallback(const EInputActionType ActionType, const FInputActionValue /*InputValue*/)
{
	if (!bGameStarted || bPlayingSequence || bIsDead)
	{
		return;
	}

	FString KeyStr;
	switch (ActionType)
	{
	case EInputActionType::EIAT_MoveUp:    KeyStr = "W"; break;
	case EInputActionType::EIAT_MoveDown:  KeyStr = "S"; break;
	case EInputActionType::EIAT_MoveLeft:  KeyStr = "A"; break;
	case EInputActionType::EIAT_MoveRight: KeyStr = "D"; break;
	default: break;
	}
	if (KeyStr.IsEmpty()) return;

	if (bQTEActive && !bInputLatched)
	{
		InputKey = KeyStr;
		bInputLatched = true;
		bWaitingResult = true;

		if (InputKey != QTEExpectedKey)
		{
			bIsSuccess = false;
			QTE_End();
			PrintResult(EQTERating::WrongKey);
			return;
		}

		const float PressNorm = QTEWindowSeconds > KINDA_SMALL_NUMBER ? (QTEElapsedSeconds / QTEWindowSeconds) : 0.f;
		if (PressNorm > 1.0f)
		{
			bIsSuccess = false;
			QTE_End();
			PrintResult(EQTERating::Miss);
			return;
		}

		const float RemNorm = 1.f - PressNorm;
		if (RemNorm <= QTERemainPerfectThreshold)
		{
			bIsSuccess = true;
			QTE_End();
			PrintResult(EQTERating::Perfect);
			return;
		}
		else if (RemNorm <= QTERemainGoodThreshold)
		{
			bIsSuccess = true;
			QTE_End();
			PrintResult(EQTERating::Good);
			return;
		}
		else
		{
			bIsSuccess = false;
			QTE_End();
			PrintResult(EQTERating::Early);
			return;
		}
	}
}

void ASLPlayerRunnerCharacter::ApplyDamage()
{
	CurrentHealth--;
	if (CurrentHealth <= 0)
	{
		OnDie();
		return;
	}
	
	if (ASLInGameHUD* InGameHUD = Cast<ASLInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		InGameHUD->ApplyPlayerHp(HPDelegate);
		InGameHUD->ApplyHitEffect((HPDelegate));
	}
	HPDelegate.OnPlayerHpChanged.Broadcast(MaxHealth, CurrentHealth);
	
	GetWorldTimerManager().SetTimer(RecoveryHpTimerHandle,
		[this]
		{
			CurrentHealth = MaxHealth;
			HPDelegate.OnPlayerHpChanged.Broadcast(MaxHealth, CurrentHealth);
	
		},
		RecoveryTime,
		false);
}

void ASLPlayerRunnerCharacter::OnOverlapedHurdle(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIdx, bool bFromSweep, const FHitResult& Hit)
{
	if (!Other || !Other->ActorHasTag(TEXT("Hurdle")) || !bWaitingResult) return;

	if (ASLRunnerHurdle* Hurdle = Cast<ASLRunnerHurdle>(Other))
	{
		if (bIsSuccess)
		{
			if (ALevelSequenceActor* SeqActor = Hurdle->GetSuccessLevelSequence())
			{
				PlayHurdleSequence(SeqActor);
			}
			else
			{
				HurdleSuccess(Hurdle->GetHurdleState(), Hurdle->GetRunnerMontageSection());
			}
		}
		else
		{
			if (ALevelSequenceActor* SeqActor = Hurdle->GetFailLevelSequence())
			{
				PlayHurdleSequence(SeqActor);
			}
			else
			{
				HurdleFail(Hurdle->GetHurdleState(), Hurdle->GetFailMontageSection());
			}
		}
	}
	bWaitingResult = false;
}

void ASLPlayerRunnerCharacter::OnDie()
{
	if (bIsDead) return;
	bIsDead = true;
	
	OnPlayerDeath.Broadcast();
	
	bGameStarted        = false;
	bSplineDriveEnabled = false;
	bPlayingSequence    = false;
	bWaitingResult      = false;
	bQTEActive          = false;
	bInputLatched       = false;

	GetWorldTimerManager().ClearTimer(ActionRootMotionTimer);
	GetWorldTimerManager().ClearTimer(IFrameTimerHandle);

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->SetMovementMode(MOVE_None);
		Move->GravityScale = 1.f;
	}

	if (USkeletalMeshComponent* Skel = GetMesh())
	{
		Skel->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		Skel->bPauseAnims = false;

		if (UAnimInstance* AI = Skel->GetAnimInstance())
		{
			AI->StopAllMontages(0.0f);
		}
	}

	SetCameraPreset(ECameraPreset::Death, 0.f);
	StartShake(HitShakeClass, 0.9f);

	checkf(DeathSequenceActor, TEXT("[Runner] DeathSequenceActor is null"));
	PlayDeathSequence();
}

void ASLPlayerRunnerCharacter::PlayDeathSequence()
{
	bPlayingSequence = true;
	checkf(DeathSequenceActor, TEXT("[Runner] DeathSequenceActor is null"));
	ActivateSequenceActor = DeathSequenceActor;

	LevelSequence = ActivateSequenceActor->GetSequencePlayer();
	if (!ensure(LevelSequence) || !DeathSequenceActor->GetSequence())
	{
		UE_LOG(LogTemp, Error, TEXT("[Runner] Death LevelSequencePlayer invalid"));
		bPlayingSequence = false;
		return;
	}

	ApplySequencePlaybackSettings_Restore(DeathSequenceActor.Get(), LevelSequence);

	LevelSequence->OnFinished.RemoveAll(this);
	LevelSequence->OnFinished.AddDynamic(this, &ASLPlayerRunnerCharacter::OnDeathSequenceFinished);

	LevelSequence->SetPlaybackPosition(FMovieSceneSequencePlaybackParams(0.f, EUpdatePositionMethod::Play));
	LevelSequence->Play();
}

void ASLPlayerRunnerCharacter::OnDeathSequenceFinished()
{
	if (LevelSequence)
	{
		LevelSequence->OnFinished.RemoveAll(this);
		LevelSequence->Stop();         
		LevelSequence = nullptr;
	}
	ActivateSequenceActor = nullptr;

	bSplineDriveEnabled = false;
	bGameStarted        = false;
}

USLRunnerAnimInstance* ASLPlayerRunnerCharacter::GetRunnerAnim() const
{
	return GetMesh() ? Cast<USLRunnerAnimInstance>(GetMesh()->GetAnimInstance()) : nullptr;
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

		Dist = FMath::Max(Dist, SplineDistOnActionStart);
		constexpr float MergeTriggerTolerance = 120.f;
		if (GoalDistance - Dist <= MergeTriggerTolerance)
		{
			Dist = GoalDistance;
		}

		MapDistanceToSegment(Dist);
		bSplineDriveEnabled = !bGoalReached;
	}

	InputKey = "";
	InputTime = 0.f;
	GetWorldTimerManager().ClearTimer(ActionRootMotionTimer);
}

void ASLPlayerRunnerCharacter::ApplyTransformAtDistance(float Distance, bool bForceSnap)
{
	if (!TrackSpline) return;

	const FRotator Rot = TrackSpline->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
	FRotator UseRot = Rot;
	if (!bFollowFullRotation) UseRot = FRotator(0.f, Rot.Yaw, 0.f);
	if (bSmoothRotation && !bForceSnap)
	{
		const FRotator Cur = GetActorRotation();
		UseRot = FMath::RInterpTo(Cur, UseRot, GetWorld()->GetDeltaSeconds(), RotationInterpSpeed);
	}

	const FVector BaseLoc = TrackSpline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
	SetActorLocationAndRotation(BaseLoc, UseRot, false, nullptr,
		bForceSnap ? ETeleportType::TeleportPhysics : ETeleportType::None);
}

void ASLPlayerRunnerCharacter::ApplyRotationAtDistance(float Distance, bool bForceSnap)
{
	if (!TrackSpline) return;

	const FRotator Rot = TrackSpline->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
	FRotator UseRot = Rot;
	if (!bFollowFullRotation) UseRot = FRotator(0.f, Rot.Yaw, 0.f);
	if (bSmoothRotation && !bForceSnap)
	{
		const FRotator Cur = GetActorRotation();
		UseRot = FMath::RInterpTo(Cur, UseRot, GetWorld()->GetDeltaSeconds(), RotationInterpSpeed);
	}
	if (bForceSnap) SetActorRotation(UseRot, ETeleportType::TeleportPhysics);
	else            SetActorRotation(UseRot);
}

void ASLPlayerRunnerCharacter::SwitchToTrackAtDistance(ASLSplineTrack* NewTrack, float StartDistance)
{
	checkf(NewTrack, TEXT("[Runner] SwitchToTrack: NewTrack is nullptr"));
	CurrentTrack  = NewTrack;
	TrackSpline   = NewTrack->GetSplineComp();
	bGoalReached  = false;
	bSplineDriveEnabled = true;

	BuildPointDistances();

	const float SplineLen = TrackSpline->GetSplineLength();
	const float Clamped   = FMath::Clamp(StartDistance, 0.f, SplineLen);

	MapDistanceToSegment(Clamped);
	ApplyTransformAtDistance(SplineDistance, /*bForceSnap=*/true);
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
	case ECameraPreset::Death:   CamTarget = Cam_Death;   break;
	default:                     CamTarget = Cam_Default; break;
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

void ASLPlayerRunnerCharacter::StartMergeToTrack(ASLSplineTrack* NewTrack)
{
	if (!NewTrack || !NewTrack->GetSplineComp()) return;

	PendingTrack  = NewTrack;
	PendingSpline = NewTrack->GetSplineComp();

	bMergingToTrack = true;
	MergeElapsed    = 0.f;

	MergeStartLoc   = GetActorLocation();
	MergeStartDir   = GetActorForwardVector();

	const float NewKey   = PendingSpline->FindInputKeyClosestToWorldLocation(MergeStartLoc);
	const float CurOnNew = PendingSpline->GetDistanceAlongSplineAtSplineInputKey(NewKey);
	PendingSplineDistance = FMath::Clamp(CurOnNew + 120.f, 0.f, PendingSpline->GetSplineLength());

	const FVector TargetLoc = PendingSpline->GetLocationAtDistanceAlongSpline(
		PendingSplineDistance, ESplineCoordinateSpace::World);

	const float worldDist    = FVector::Dist(MergeStartLoc, TargetLoc);
	const float worldSpeed   = FMath::Max(1.f, ForwardSpeed);
	const float durBySpeed   = worldDist / worldSpeed;

	TrackBlendDuration = FMath::Clamp(durBySpeed, 0.15f, 1.2f);

	bSplineDriveEnabled = false;

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->SetMovementMode(MOVE_Flying);
	}
}

void ASLPlayerRunnerCharacter::TickMergeToTrack(float DeltaSeconds)
{
	if (!bMergingToTrack || !PendingSpline) return;

	MergeElapsed += DeltaSeconds;
	const float Alpha = FMath::Clamp(MergeElapsed / FMath::Max(0.001f, TrackBlendDuration), 0.f, 1.f);
	const float W = EvalMergeWeight(Alpha);

	const FTransform TargetWS = PendingSpline->GetTransformAtDistanceAlongSpline(PendingSplineDistance, ESplineCoordinateSpace::World);
	FRotator TargetRot = TargetWS.Rotator();
	if (!bFollowFullRotation) TargetRot = FRotator(0.f, TargetRot.Yaw, 0.f);

	const FVector TargetLoc = TargetWS.GetLocation();
	const FVector NewLoc = FMath::Lerp(MergeStartLoc, TargetLoc, W);
	const FQuat   NewRot = FQuat::Slerp(GetActorQuat(), TargetRot.Quaternion(), W);

	SetActorLocationAndRotation(NewLoc, NewRot.Rotator(), false, nullptr, ETeleportType::TeleportPhysics);

	if (Alpha >= 1.f - KINDA_SMALL_NUMBER)
	{
		bMergingToTrack = false;
		PendingSpline   = nullptr;

		if (UCharacterMovementComponent* Move = GetCharacterMovement())
		{
			Move->SetMovementMode(MOVE_Walking);
		}

		SwitchToTrackAtDistance(PendingTrack, PendingSplineDistance);
		PendingTrack = nullptr;
	}
}

float ASLPlayerRunnerCharacter::EvalMergeWeight(float Alpha) const
{
	return FMath::InterpEaseInOut(0.f, 1.f, Alpha, 2.0f);
}

void ASLPlayerRunnerCharacter::StartTrackBlend(ASLSplineTrack* NewTrack, float Duration)
{
	if (!NewTrack || !NewTrack->GetSplineComp()) return;

	bSplineDriveEnabled = false;
	bTrackBlendActive   = true;
	TrackBlendAlpha     = 0.f;
	TrackBlendDuration  = FMath::Max(Duration, 0.f);
	TrackBlendStart     = GetActorTransform();

	const USplineComponent* NextSpline = NewTrack->GetSplineComp();
	const FTransform TargetWS = NextSpline->GetTransformAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
	FRotator UseRot = TargetWS.Rotator();
	if (!bFollowFullRotation) UseRot = FRotator(0.f, UseRot.Yaw, 0.f);
	TrackBlendTarget = FTransform(UseRot, TargetWS.GetLocation(), FVector(1,1,1));

	PendingTrack = NewTrack;

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->SetMovementMode(MOVE_None);
	}
}

void ASLPlayerRunnerCharacter::ApplySequencePlaybackSettings_Restore(ALevelSequenceActor* SeqActor, ULevelSequencePlayer* Player)
{
	if (SeqActor)
	{
		SeqActor->PlaybackSettings.FinishCompletionStateOverride =
			EMovieSceneCompletionModeOverride::ForceRestoreState;
	}
	if (Player)
	{
		FMovieSceneSequencePlaybackSettings S =
			SeqActor ? SeqActor->PlaybackSettings : FMovieSceneSequencePlaybackSettings{};
		S.FinishCompletionStateOverride = EMovieSceneCompletionModeOverride::ForceRestoreState;
		Player->SetPlaybackSettings(S);
	}
}

void ASLPlayerRunnerCharacter::ResetComponentsAfterSequence()
{
	if (USkeletalMeshComponent* M = GetMesh())
	{
		M->SetRelativeTransform(MeshDefaultRelative, false, nullptr, ETeleportType::TeleportPhysics);
	}
	if (SpringArm)
	{
		SpringArm->SetRelativeTransform(SpringArmDefaultRelative, false, nullptr, ETeleportType::TeleportPhysics);
	}
	if (FollowCamera)
	{
		FollowCamera->SetRelativeTransform(CameraDefaultRelative, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void ASLPlayerRunnerCharacter::AlignToSplineFromTransform(const FTransform& WorldTM)
{
	if (!TrackSpline) return;

	const float Key  = TrackSpline->FindInputKeyClosestToWorldLocation(WorldTM.GetLocation());
	const float Dist = TrackSpline->GetDistanceAlongSplineAtSplineInputKey(Key);

	MapDistanceToSegment(Dist);
	ApplyTransformAtDistance(SplineDistance, /*bForceSnap=*/true);
}

void ASLPlayerRunnerCharacter::PlayEntrySequence()
{
	bPlayingSequence = true;

	ActivateSequenceActor = EntrySequenceActor;
	LevelSequence = ActivateSequenceActor.IsValid() ? ActivateSequenceActor->GetSequencePlayer() : nullptr;
	if (!ActivateSequenceActor.IsValid() || !LevelSequence || !ActivateSequenceActor->GetSequence())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Runner] Entry Seq invalid. Resume immediately."));
		bPlayingSequence = false;
		ResumeAfterSequenceImmediate(true);
		return;
	}

	ApplySequencePlaybackSettings_Restore(ActivateSequenceActor.Get(), LevelSequence);

	LevelSequence->OnFinished.RemoveAll(this);
	LevelSequence->OnFinished.AddDynamic(this, &ASLPlayerRunnerCharacter::OnEntrySequenceFinished);

	LevelSequence->SetPlaybackPosition(FMovieSceneSequencePlaybackParams(0.f, EUpdatePositionMethod::Play));
	LevelSequence->Play();
}

void ASLPlayerRunnerCharacter::OnEntrySequenceFinished()
{
	if (LevelSequence)
	{
		LevelSequence->OnFinished.RemoveAll(this);
		LevelSequence->Stop();
		LevelSequence = nullptr;
	}
	ActivateSequenceActor = nullptr;

	if (bLastSeqTransformValid) AlignToSplineFromTransform(LastSequenceWorldTransform);
	else                        AlignToSplineFromTransform(GetActorTransform());

	ResetComponentsAfterSequence();

	ResumeAfterSequenceImmediate(true);
}

void ASLPlayerRunnerCharacter::PlayHurdleSequence(ALevelSequenceActor* SequenceActor)
{
	checkf(SequenceActor, TEXT("ASLPlayerRunnerCharacter Missing Sequence"));
	bPlayingSequence = true;

	ULevelSequencePlayer* Player = SequenceActor->GetSequencePlayer();
	if (!SequenceActor->GetSequence() || !Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Runner] Hurdle Seq invalid. Fallback to resume."));
		bPlayingSequence = false;
		ResumeAfterSequenceImmediate(true);
		return;
	}

	ApplySequencePlaybackSettings_Restore(SequenceActor, Player);

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->SetMovementMode(MOVE_None);
	}
	if (USLRunnerAnimInstance* Anim = GetRunnerAnim())
	{
		Anim->bIsPlayingSequence = true;
	}

	ActivateSequenceActor = SequenceActor;
	LevelSequence = Player;

	LevelSequence->OnFinished.RemoveAll(this);
	LevelSequence->OnFinished.AddDynamic(this, &ASLPlayerRunnerCharacter::OnHurdleSequenceFinished);

	LevelSequence->SetPlaybackPosition(FMovieSceneSequencePlaybackParams(0.f, EUpdatePositionMethod::Play));
	LevelSequence->Play();
}

void ASLPlayerRunnerCharacter::OnHurdleSequenceFinished()
{
	if (LevelSequence)
	{
		LevelSequence->OnFinished.RemoveAll(this);
		LevelSequence->Stop();
		LevelSequence = nullptr;
	}
	ActivateSequenceActor = nullptr;
	
	if (bLastSeqTransformValid) AlignToSplineFromTransform(LastSequenceWorldTransform);
	else                        AlignToSplineFromTransform(GetActorTransform());

	ResetComponentsAfterSequence();

	ResumeAfterSequenceImmediate(true);
}
void ASLPlayerRunnerCharacter::PlayTransitionTrackSequence(ALevelSequenceActor* SequenceActor)
{
	checkf(SequenceActor, TEXT("ASLPlayerRunnerCharacter Missing Sequence"));

	if (bPlayingSequence) return;

	bPlayingSequence   = true;

	if (!SequenceActor->GetSequence())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Runner] Transition SeqActor has no sequence asset. Fallback to merge."));
		bPlayingSequence = false;

		if (ensure(CurrentTrack) && ensure(CurrentTrack->GetNextTrack()))
		{
			StartMergeToTrack(CurrentTrack->GetNextTrack());
		}
		return;
	}

	ULevelSequencePlayer* Player = SequenceActor->GetSequencePlayer();
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Runner] Transition SeqActor has no SequencePlayer. Fallback to merge."));
		bPlayingSequence = false;

		if (ensure(CurrentTrack) && ensure(CurrentTrack->GetNextTrack()))
		{
			StartMergeToTrack(CurrentTrack->GetNextTrack());
		}
		return;
	}

	ApplySequencePlaybackSettings_Restore(SequenceActor, Player);

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->SetMovementMode(MOVE_None);
	}

	ActivateSequenceActor = SequenceActor;
	LevelSequence = Player;

	LevelSequence->OnFinished.RemoveAll(this);
	LevelSequence->OnFinished.AddDynamic(this, &ASLPlayerRunnerCharacter::OnTransitionSequenceFinished);

	LevelSequence->SetPlaybackPosition(FMovieSceneSequencePlaybackParams(0.f, EUpdatePositionMethod::Play));
	LevelSequence->Play();
}

void ASLPlayerRunnerCharacter::OnTransitionSequenceFinished()
{
	if (LevelSequence)
	{
		LevelSequence->OnFinished.RemoveAll(this);
		LevelSequence->Stop();
		LevelSequence = nullptr;
	}
	ActivateSequenceActor = nullptr;

	if (bLastSeqTransformValid) AlignToSplineFromTransform(LastSequenceWorldTransform);
	else                        AlignToSplineFromTransform(GetActorTransform());

	ResetComponentsAfterSequence();

	if (ensure(CurrentTrack) && ensure(CurrentTrack->GetNextTrack()))
	{
		SwitchToTrackAtDistance(CurrentTrack->GetNextTrack(), PendingSplineDistance);
	}

	ResumeAfterSequenceImmediate(true);
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
	if (IsUpperBodyMontage(Montage)) return;

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

bool ASLPlayerRunnerCharacter::IsUpperBodyMontage(const UAnimMontage* Montage) const
{
	if (!Montage) return false;

	for (const FSlotAnimationTrack& Track : Montage->SlotAnimTracks)
	{
		if (Track.SlotName == SLOT_UpperBody)
		{
			return true;
		}
	}
	return false;
}

void ASLPlayerRunnerCharacter::ResumeAfterSequenceImmediate(bool bPlayRunStart)
{
	if (USkeletalMeshComponent* Skel = GetMesh())
	{
		Skel->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		Skel->bPauseAnims = false;

		if (UAnimInstance* AI = Skel->GetAnimInstance())
		{
			AI->StopAllMontages(0.0f);
		}
	}

	if (USLRunnerAnimInstance* RunAnim = GetRunnerAnim())
	{
		RunAnim->bIsPlayingSequence = false;
		if (bPlayRunStart)
		{
			RunAnim->PlayRunStart();
		}
	}

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->SetMovementMode(MOVE_Walking);
		Move->StopMovementImmediately();
	}

	bPlayingSequence    = false;
	bGoalReached        = false;
	bSplineDriveEnabled = true;
	bGameStarted        = true;

	bLastSeqTransformValid = false;
}

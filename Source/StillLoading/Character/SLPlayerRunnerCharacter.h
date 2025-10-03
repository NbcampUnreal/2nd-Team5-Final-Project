#pragma once

#include "UI/Struct/SLInGameDelegateBuffers.h"
#include "Character/RunnerTypes.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Minigame/System/SLSplineTrack.h"
#include "SLPlayerRunnerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerSusccess);

class ULevelSequencePlayer;
class UDynamicIMCComponent;
class UBoxComponent;
class USLRunnerAnimInstance;
class USplineComponent;
class UAnimMontage;
class USpringArmComponent;
class UCameraComponent;
class UCameraShakeBase;
class ASLSplineTrack;
class ALevelSequenceActor;
struct FInputActionValue;
struct FSLPlayerHpDelegateBuffer;
enum class EInputActionType : uint8;

UENUM(BlueprintType)
enum class ECameraPreset : uint8
{
	Default,
	Slide,
	Jump,
	Attack,
	Hit,
	Death
};

USTRUCT(BlueprintType)
struct FRunnerCamPreset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Camera")
	float TargetArmLength = 350.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Camera")
	FVector SocketOffset = FVector(0, 0, 80);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Camera")
	FRotator ArmRotation = FRotator(-10, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Camera")
	float FOV = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Camera")
	float BlendSpeed = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Camera")
	float ShakeAmplitude = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Camera")
	float ShakeFrequency = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Camera")
	float YawAroundCharacter = 0.f;
};

UCLASS()
class STILLLOADING_API ASLPlayerRunnerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASLPlayerRunnerCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category="Runner|Spline")
	USplineComponent* GetTrackSpline() const { return CurrentTrack->GetSplineComp(); }

	UFUNCTION(BlueprintCallable, Category="Runner|Spline")
	void SnapToNearestOnSpline();

	UFUNCTION(BlueprintCallable, Category="Runner|State")
	FString GetInputKey() const { return InputKey; }

	UFUNCTION(BlueprintCallable, Category="Runner|State")
	void HurdleSuccess(EHurdleState ObState, ERunnerMontageSection Section);

	UFUNCTION(BlueprintCallable, Category="Runner|State")
	void HurdleFail(EHurdleState ObState, ERunnerMontageSection Section);

	UFUNCTION(BlueprintCallable, Category="Runner|QTE")
	void QTE_Begin(const FString& ExpectedKey, float WindowSeconds, EHurdleState State, ERunnerMontageSection SuccessSection, ERunnerMontageSection FailSection);

	UFUNCTION(BlueprintCallable, Category="Runner|QTE")
	void QTE_End();

	UFUNCTION(BlueprintCallable, Category="Runner|Sequence")
	void PlayEntrySequence();

	UFUNCTION(BlueprintCallable, Category="Runner|Sequence")
	void PlayDeathSequence();
	
	UFUNCTION(BlueprintCallable, Category="Runner|Sequence")
	void PlayHurdleSequence(ALevelSequenceActor* SequenceActor);

	UFUNCTION(BlueprintCallable, Category="Runner|Sequence")
	void PlayTransitionTrackSequence(ALevelSequenceActor* SequenceActor);

protected:
	UFUNCTION()
	void OnActionTriggeredCallback(const EInputActionType ActionType, const FInputActionValue InputValue);

	UFUNCTION(BlueprintCallable, Category="Runner|State")
	void ApplyDamage();

	UFUNCTION(BlueprintImplementableEvent, Category="Runner|State")
	void OnHitVFX();
	
	UFUNCTION(BlueprintCallable, Category="Runner|State")
	void OnDie();

	UFUNCTION(BlueprintImplementableEvent, Category="Runner|State")
	void PrintResult(EQTERating Result);

private:
	USLRunnerAnimInstance* GetRunnerAnim() const;

	void StartIFrame(float Duration);

	UFUNCTION(BlueprintCallable, Category="Runner|State")
	void EnterRootMotionAction(float ExpectedDuration);

	void ExitRootMotionAction();

	void ApplyTransformAtDistance(float Distance, bool bForceSnap);
	void ApplyRotationAtDistance(float Distance, bool bForceSnap);
	void ApplyTransformAtDistance(float Distance) { ApplyTransformAtDistance(Distance, false); }
	void ApplyRotationAtDistance(float Distance)  { ApplyRotationAtDistance(Distance,  false); }

	UFUNCTION()
	void OnHurdleSequenceFinished();

	UFUNCTION()
	void OnTransitionSequenceFinished();

	UFUNCTION()
	void OnEntrySequenceFinished();

	UFUNCTION()
	void OnDeathSequenceFinished();

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageBlendingOutStarted(UAnimMontage* Montage, bool bInterrupted);

	bool IsHitMontage(const UAnimMontage* Montage) const;
	bool IsUpperBodyMontage(const UAnimMontage* Montage) const;

	void BuildPointDistances();
	void MapDistanceToSegment(float Distance);
	void AdvanceAlongSegment(float DeltaSeconds);
	void ReachGoal();

	void SetCameraPreset(ECameraPreset Preset, float HoldTime = 0.f);
	void ApplyCamera(float DeltaSeconds);
	void StartShake(TSubclassOf<UCameraShakeBase> ShakeClass, float Scale = 1.f);

	UFUNCTION()
	void OnOverlapedHurdle(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIdx, bool bFromSweep, const FHitResult& Hit);

	UFUNCTION()
	void StartMergeToTrack(class ASLSplineTrack* NewTrack);

	UFUNCTION()
	void TickMergeToTrack(float DeltaSeconds);

	float EvalMergeWeight(float Alpha) const;

	void StartTrackBlend(ASLSplineTrack* NewTrack, float Duration);

	void SwitchToTrackAtDistance(ASLSplineTrack* NewTrack, float StartDistance);

	void ResumeAfterSequenceImmediate(bool bPlayRunStart);

	void ApplySequencePlaybackSettings_Restore(ALevelSequenceActor* SeqActor, ULevelSequencePlayer* Player);

	void ResetComponentsAfterSequence();

	void AlignToSplineFromTransform(const FTransform& WorldTM);

	bool IsCamHoldActive() const
	{
		return GetWorld() && (GetWorld()->GetTimeSeconds() < CamHoldUntilTime);
	}
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Camera")
	FRunnerCamPreset Cam_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Camera")
	FRunnerCamPreset Cam_Slide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Camera")
	FRunnerCamPreset Cam_Jump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Camera")
	FRunnerCamPreset Cam_Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Camera")
	FRunnerCamPreset Cam_Hit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Camera")
	FRunnerCamPreset Cam_Death;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Camera")
	TSubclassOf<UCameraShakeBase> JumpShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Camera")
	TSubclassOf<UCameraShakeBase> SlideShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Camera")
	TSubclassOf<UCameraShakeBase> HitShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Camera")
	TSubclassOf<UCameraShakeBase> AttackShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Spline")
	float ForwardSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Spline")
	bool bStartAtSplineStart = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Spline")
	bool bFollowFullRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Spline", meta=(EditCondition="bFollowFullRotation==true"))
	bool bSmoothRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Spline", meta=(EditCondition="bSmoothRotation", ClampMin="0.0"))
	float RotationInterpSpeed = 10.f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Runner|Spline")
	TObjectPtr<ASLSplineTrack> CurrentTrack = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Sequence")
	TObjectPtr<ALevelSequenceActor> EntrySequenceActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Sequence")
	TObjectPtr<ALevelSequenceActor> DeathSequenceActor;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerDeath OnPlayerDeath;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerSusccess OnPlayerSusccess;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Components")
	TObjectPtr<UBoxComponent> BoxComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Components")
	TObjectPtr<UDynamicIMCComponent> DynamicIMCComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Input")
	FString InputKey;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Input")
	float InputTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Health")
	int32 MaxHealth = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Health")
	int32 CurrentHealth = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Health")
	float RecoveryTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Combat")
	float IFrameDuration = 0.2f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Runner|Spline")
	float SplineDistance = 0.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Runner|Spline")
	bool bSplineDriveEnabled = true;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Runner|Spline")
	bool bGoalReached = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Runner|Spline")
	bool bPlayingSequence = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Runner|Spline")
	float GoalDistance = 0.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Runner|Spline")
	int32 LastPointIndex = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Runner|Spline")
	int32 CurrentPointIndex = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Runner|Spline")
	float CurrentSegmentEndDistance = 0.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Runner|Spline")
	TArray<float> PointDistances;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Camera")
	ECameraPreset ActivePreset = ECameraPreset::Default;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Camera")
	FRunnerCamPreset CamTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Camera")
	FRunnerCamPreset CamCurrent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Camera")
	float CamNoiseTime = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Track")
	TObjectPtr<USplineComponent> TrackSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Transition")
	float TrackBlendDurationDefault = 0.6f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Transition")
	bool bTrackBlendActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Transition")
	float TrackBlendAlpha = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Transition")
	float TrackBlendDuration = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Transition")
	FTransform TrackBlendStart;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Transition")
	FTransform TrackBlendTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Transition")
	TObjectPtr<ASLSplineTrack> PendingTrack = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|QTE")
	float QTERemainPerfectThreshold = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|QTE")
	float QTERemainGoodThreshold = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|QTE")
	float QTENormalizedLateGrace = 1.01f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|QTE")
	bool bQTEActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|QTE")
	bool bInputLatched = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|QTE")
	bool bWaitingResult = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|QTE")
	bool bIsSuccess = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|QTE")
	FString QTEExpectedKey;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|QTE")
	float QTEWindowSeconds = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|QTE")
	float QTEElapsedSeconds = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|QTE")
	EHurdleState QTEState = EHurdleState::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|QTE")
	ERunnerMontageSection QTESuccessSection = ERunnerMontageSection::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|QTE")
	ERunnerMontageSection QTEFailSection = ERunnerMontageSection::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|State")
	bool bGameStarted = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|State")
	bool bInvincible = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|State")
	bool bIsDead = false;

	UPROPERTY(Transient)
	TObjectPtr<ULevelSequencePlayer> LevelSequence = nullptr;

	UPROPERTY(Transient)
	TWeakObjectPtr<ALevelSequenceActor> ActivateSequenceActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Runtime")
	float SplineDistOnActionStart = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Runtime")
	bool bMergingToTrack = false;

	UPROPERTY()
	TObjectPtr<USplineComponent> PendingSpline = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Runtime")
	float MergeElapsed = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Runtime")
	float PendingSplineDistance = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Runtime")
	FVector MergeStartLoc = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Runtime")
	FVector MergeStartDir = FVector::ForwardVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Runtime")
	float MergeFreeTravelDist = 0.f;

	UPROPERTY(Transient)
	FTransform LastSequenceWorldTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|State")
	bool bLastSeqTransformValid = false;

	UPROPERTY(Transient)
	FTransform MeshDefaultRelative;

	UPROPERTY(Transient)
	FTransform SpringArmDefaultRelative;

	UPROPERTY(Transient)
	FTransform CameraDefaultRelative;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Camera")
	float CamHoldUntilTime = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Camera")
	ECameraPreset HeldPreset = ECameraPreset::Default;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Camera")
	int32 RootMotionCamDepth = 0;



private:
	FTimerHandle ActionRootMotionTimer;
	FTimerHandle CamPresetTimer;
	FTimerHandle RecoveryHpTimerHandle;
	FTimerHandle IFrameTimerHandle;

	
	UPROPERTY()
	FSLPlayerHpDelegateBuffer HPDelegate;
	double RealPrev = 0.0;
};

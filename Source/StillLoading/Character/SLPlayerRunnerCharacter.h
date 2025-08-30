#pragma once

#include "Character/RunnerTypes.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SLPlayerRunnerCharacter.generated.h"

class ULevelSequence;
class ALevelSequenceActor;
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
struct FInputActionValue;
enum class EInputActionType : uint8;

UENUM(BlueprintType)
enum class ECameraPreset : uint8
{
	Default, Slide, Jump, Attack, Hit
};

USTRUCT(BlueprintType)
struct FRunnerCamPreset
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TargetArmLength = 350.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector SocketOffset = FVector(0,0,80);
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator ArmRotation = FRotator(-10,0,0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FOV = 90.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BlendSpeed = 8.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShakeAmplitude = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShakeFrequency = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float YawAroundCharacter = 0.f;
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
	void SnapToNearestOnSpline();

protected:
	UFUNCTION()
	void OnActionTriggeredCallback(const EInputActionType ActionType, const FInputActionValue InputValue);

	UFUNCTION()
	void OnActionStartedCallback(EInputActionType ActionType);

	UFUNCTION()
	void OnActionCompletedCallback(EInputActionType ActionType);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintImplementableEvent)
	void ApplyDamage();
	UFUNCTION(BlueprintCallable)
	void OnDie();

private:
	USLRunnerAnimInstance* GetRunnerAnim() const;

	void StartIFrame(float Duration);

	UFUNCTION(BlueprintCallable, Category="Runner|State")
	void EnterRootMotionAction(float ExpectedDuration);
	void ExitRootMotionAction();

	void ApplyTransformAtDistance(float Distance);
	void ApplyRotationAtDistance(float Distance);

	UFUNCTION()
	void SwitchToTrack(ASLSplineTrack* NewTrack);
	UFUNCTION()
	void PlayTransitionTrackSequence(ULevelSequence* Sequence);
	UFUNCTION()
	void OnTransitionSequenceFinished();
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageBlendingOutStarted(UAnimMontage* Montage, bool bInterrupted);

	bool IsHitMontage(const UAnimMontage* Montage) const;

	void BuildPointDistances();
	void MapDistanceToSegment(float Distance);
	void AdvanceAlongSegment(float DeltaSeconds);
	void ReachGoal();

	void SetCameraPreset(ECameraPreset Preset, float HoldTime = 0.f);
	void ApplyCamera(float DeltaSeconds);
	void StartShake(TSubclassOf<UCameraShakeBase> ShakeClass, float Scale = 1.f);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Strafe")
	float LateralSpeed = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Strafe")
	float LateralClamp = 180.f;

	UPROPERTY(EditInstanceOnly, Category="Runner|Spline")
	TObjectPtr<ASLSplineTrack> CurrentTrack = nullptr;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Components")
	TObjectPtr<UBoxComponent> BoxComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Components")
	TObjectPtr<UDynamicIMCComponent> DynamicIMCComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|State")
	EHurdleState CurrentState = EHurdleState::None;

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
	bool bPlayingTransitionSequence = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Runner|Spline")
	float GoalDistance = 0.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Runner|Spline")
	int32 LastPointIndex = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Runner|Spline")
	int32 CurrentPointIndex = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Runner|Spline")
	float CurrentSegmentEndDistance = 0.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Runner|Strafe")
	float LateralOffset = 0.f;

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
	float CurrentLateralInput = 0.f;
	bool bInvincible = false;

	FTimerHandle StateTimerHandle;
	FTimerHandle RecoveryTimerHandle;
	FTimerHandle IFrameTimerHandle;

	float SplineDistOnActionStart = 0.f;

private:
	FTimerHandle ActionRootMotionTimer;
	FTimerHandle CamPresetTimer;

	UPROPERTY(Transient)
	TObjectPtr<ULevelSequencePlayer> TransitionSequence = nullptr;
	UPROPERTY(Transient)
	TWeakObjectPtr<ALevelSequenceActor> ActivateSequenceActor = nullptr;
};
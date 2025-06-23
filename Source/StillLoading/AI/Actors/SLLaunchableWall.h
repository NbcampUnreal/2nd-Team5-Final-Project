#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SLLaunchableWall.generated.h"

class ASLMouseActor;
class UNiagaraSystem;
class ASLLaunchableWall;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLOnAllWallPartsLaunched, ASLLaunchableWall*, LaunchedWall);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnWallAnimationCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLOnWallPartRotationCompleted, int32, WallPartIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnWallYMovementCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnWallSpacingCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSLOnWallHitMouseActor, ASLMouseActor*, HitMouseActor, int32, WallPartIndex);

UENUM(BlueprintType)
enum class EWallPartState : uint8
{
	Inactive	UMETA(DisplayName = "Inactive"),
	Rotating	UMETA(DisplayName = "Rotating"),
	AimingToPlayer	UMETA(DisplayName = "Aiming To Player"),
	Launched	UMETA(DisplayName = "Launched")
};

UCLASS()
class STILLLOADING_API ASLLaunchableWall : public AActor
{
	GENERATED_BODY()

public:
	ASLLaunchableWall();

	// Public Functions
	UFUNCTION(BlueprintCallable, Category = "LaunchableWall")
	void LaunchWallToPlayer();

	UFUNCTION(BlueprintCallable, Category = "LaunchableWall")
	bool CanLaunch() const;

	UFUNCTION(BlueprintCallable, Category = "LaunchableWall")
	void RefreshWallMeshes();

	UFUNCTION(BlueprintCallable, Category = "LaunchableWall")
	void RefreshWallLayout();

	UFUNCTION(BlueprintCallable, Category = "LaunchableWall")
	void ApplySpawnRotation();
	
	UFUNCTION(BlueprintCallable, Category = "LaunchableWall")
	void ResetWall();

	UFUNCTION(BlueprintCallable, Category = "LaunchableWall")
	void SetAutoResetEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "LaunchableWall")
	bool IsAutoResetEnabled() const { return bAutoResetEnabled; }
	
	// Public Variables (Delegates)
	UPROPERTY(BlueprintAssignable, Category = "LaunchableWall")
	FSLOnAllWallPartsLaunched OnAllWallPartsLaunched;

	UPROPERTY(BlueprintAssignable, Category = "LaunchableWall")
	FSLOnWallAnimationCompleted OnWallAnimationCompleted;

	UPROPERTY(BlueprintAssignable, Category = "LaunchableWall")
	FSLOnWallPartRotationCompleted OnWallPartRotationCompleted;

	UPROPERTY(BlueprintAssignable, Category = "LaunchableWall")
	FSLOnWallYMovementCompleted OnWallYMovementCompleted;

	UPROPERTY(BlueprintAssignable, Category = "LaunchableWall")
	FSLOnWallSpacingCompleted OnWallSpacingCompleted;

	UPROPERTY(BlueprintAssignable, Category = "LaunchableWall")
	FSLOnWallHitMouseActor OnWallHitMouseActor;

protected:
	// Protected Functions
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	void OnWallPartHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnRotationTimelineUpdate(float Value);
	
	UFUNCTION()
	void OnRotationTimelineFinished();

	UFUNCTION()
	void OnPlayerAimTimelineUpdate(float Value);
	
	UFUNCTION()
	void OnPlayerAimTimelineFinished();

	UFUNCTION()
	void OnYMovementTimelineUpdate(float Value);
	
	UFUNCTION()
	void OnYMovementTimelineFinished();
	
	UFUNCTION()
	void OnSpacingTimelineUpdate(float Value);
	
	UFUNCTION()
	void OnSpacingTimelineFinished();

	void SetupWallParts();
	void LaunchNextPart();
	void CheckAllPartsLaunched();
	void RotateCurrentPart();
	void LaunchCurrentPart();
	void StartSmoothRotation();
	void StartPlayerAimRotation();
	void StartPreLaunchAnimation();
	void StartYMovementAnimation();
	void StartSpacingAnimation();
	void UpdateWallPartsYPosition(float AnimationProgress);
	void UpdateWallPartsSpacing(float AnimationProgress);
	void ApplyDamageToPlayer(AActor* PlayerActor, const FHitResult& HitResult, int32 WallPartIndex);
	void PlayCharacterHitEffects(const FVector& HitLocation, AActor* HitActor);
	void PlayGroundHitEffects(const FVector& HitLocation);
	void DestroyWallPart(int32 WallPartIndex);
	FVector GetPlayerLocation() const;
	FVector GetPlayerDirection() const;
	FVector GetPlayerDirectionFromPoint(const FVector& FromPoint) const; 
	void ApplySpawnRotationToWallPart(UStaticMeshComponent* WallPart, int32 PartIndex);
	void SetupWallPartCollisions(UStaticMeshComponent* WallPart);
	bool ShouldIgnoreCollision(AActor* OtherActor) const;
	void HandleMouseActorHit(ASLMouseActor* MouseActor, const FHitResult& HitResult, int32 WallPartIndex);

	// Protected Variables (Components)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TArray<TObjectPtr<UStaticMeshComponent>> WallParts;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TArray<TObjectPtr<UProjectileMovementComponent>> ProjectileMovements;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTimelineComponent> RotationTimeline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTimelineComponent> PlayerAimTimeline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTimelineComponent> YMovementTimeline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTimelineComponent> SpacingTimeline;

	// Protected Variables (Settings)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Settings", meta = (ClampMin = "1", ClampMax = "20"))
	int32 NumberOfWallParts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Settings")
	float WallPartSpacing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Settings")
	float LaunchDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Settings")
	float LaunchSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Settings")
	FVector WallPartScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Settings")
	bool bArrangeHorizontally;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Settings")
	float ProjectileGravityScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation Settings")
	float RotationDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation Settings", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float RotationDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation Settings")
	TObjectPtr<UCurveFloat> RotationCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Aim Settings")
	float PlayerAimDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Aim Settings")
	TObjectPtr<UCurveFloat> PlayerAimCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Rotation")
	FRotator SpawnRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Rotation")
	bool bApplySpawnRotationOnConstruction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pre-Launch Animation")
	bool bEnablePreLaunchAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pre-Launch Animation", meta = (EditCondition = "bEnablePreLaunchAnimation"))
	float YMovementDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pre-Launch Animation", meta = (EditCondition = "bEnablePreLaunchAnimation"))
	float SpacingExpansionDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pre-Launch Animation", meta = (EditCondition = "bEnablePreLaunchAnimation", ClampMin = "0.1", ClampMax = "3.0"))
	float YMovementDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pre-Launch Animation", meta = (EditCondition = "bEnablePreLaunchAnimation", ClampMin = "0.1", ClampMax = "3.0"))
	float SpacingAnimationDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pre-Launch Animation", meta = (EditCondition = "bEnablePreLaunchAnimation"))
	TObjectPtr<UCurveFloat> YMovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pre-Launch Animation", meta = (EditCondition = "bEnablePreLaunchAnimation"))
	TObjectPtr<UCurveFloat> SpacingAnimationCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects|Character")
	TObjectPtr<UNiagaraSystem> CharacterHitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects|Character")
	TObjectPtr<USoundBase> CharacterHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects|Ground")
	TObjectPtr<UNiagaraSystem> GroundHitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects|Ground")
	TObjectPtr<USoundBase> GroundHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Settings")
	TObjectPtr<UStaticMesh> DefaultWallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Settings")
	bool bUseCustomMeshPerPart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Settings", meta = (EditCondition = "bUseCustomMeshPerPart"))
	TArray<TObjectPtr<UStaticMesh>> WallPartMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Actor")
	float MouseActorStunDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Actor")
	TObjectPtr<UNiagaraSystem> MouseHitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Actor")
	TObjectPtr<USoundBase> MouseHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Settings", meta = (ClampMin = "0.0"))
	float PostLaunchLifetime;
	
private:
	// Private Functions
	void CreateWallParts();
	void CreateWallPart(int32 Index);
	void UpdateWallPartsVisibility();
	UStaticMesh* GetMeshForPart(int32 Index) const;

	// Private Variables
	static const int32 MaxWallParts = 5;

	int32 CurrentLaunchIndex;
	int32 LaunchedPartsCount;
	bool bIsLaunching;
	bool bIsYMovementAnimating;
	bool bIsSpacingAnimating;
	int32 CurrentRotatingPartIndex;

	TArray<EWallPartState> WallPartStates;
	TArray<FVector> OriginalWallPartPositions;
	TArray<FVector> YMovedWallPartPositions;

	FOnTimelineFloat RotationTimelineUpdateDelegate;
	FOnTimelineEvent RotationTimelineFinishedDelegate;
	FOnTimelineFloat PlayerAimTimelineUpdateDelegate;
	FOnTimelineEvent PlayerAimTimelineFinishedDelegate;
	FOnTimelineFloat YMovementTimelineUpdateDelegate;
	FOnTimelineEvent YMovementTimelineFinishedDelegate;
	FOnTimelineFloat SpacingTimelineUpdateDelegate;
	FOnTimelineEvent SpacingTimelineFinishedDelegate;

	FRotator StartRotation;
	FRotator TargetRotation;
	FRotator PlayerAimStartRotation;
	FRotator PlayerAimTargetRotation;

	bool bAutoResetEnabled;
};
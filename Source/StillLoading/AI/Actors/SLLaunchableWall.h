#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SLLaunchableWall.generated.h"

// 카테고리 정의
#define CATEGORY_COMPONENTS "LaunchableWall | Components"
#define CATEGORY_WALL_SETTINGS "LaunchableWall | Wall Settings"
#define CATEGORY_WALL "LaunchableWall | Wall"
#define CATEGORY_MESH "LaunchableWall | Mesh"
#define CATEGORY_WALL_LAYOUT "LaunchableWall | Wall Layout"
#define CATEGORY_ROTATION_SETTINGS "LaunchableWall | Rotation Settings"
#define CATEGORY_PLAYER_AIM_SETTINGS "LaunchableWall | Player Aim Settings"
#define CATEGORY_SPAWN_ROTATION "LaunchableWall | Spawn Rotation"
#define CATEGORY_PRE_LAUNCH_ANIMATION "LaunchableWall | Pre-Launch Animation"
#define CATEGORY_EFFECTS_CHARACTER "LaunchableWall | Effects|Character"
#define CATEGORY_EFFECTS_GROUND "LaunchableWall | Effects|Ground"
#define CATEGORY_MESH_SETTINGS "LaunchableWall | Mesh Settings"
#define CATEGORY_EVENTS "LaunchableWall | Events"
#define CATEGORY_RUNTIME_DATA "LaunchableWall | Runtime Data"

class UNiagaraSystem;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnAllWallPartsLaunched);

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

	UFUNCTION(BlueprintCallable, Category = CATEGORY_WALL)
	void LaunchWallToPlayer();

	UFUNCTION(BlueprintCallable, Category = CATEGORY_WALL)
	bool CanLaunch() const;

	UFUNCTION(BlueprintCallable, Category = CATEGORY_MESH)
	void RefreshWallMeshes();

	UFUNCTION(BlueprintCallable, Category = CATEGORY_WALL_LAYOUT)
	void RefreshWallLayout();

	UFUNCTION(BlueprintCallable, Category = CATEGORY_SPAWN_ROTATION)
	void ApplySpawnRotation();

	UFUNCTION(BlueprintCallable, Category = CATEGORY_SPAWN_ROTATION)
	void SetSpawnRotation(const FRotator& NewRotation);

	// Runtime Data 변경 함수들
	UFUNCTION(BlueprintCallable, Category = CATEGORY_RUNTIME_DATA)
	void SetLaunchSpeed(float NewSpeed);

	UFUNCTION(BlueprintCallable, Category = CATEGORY_RUNTIME_DATA)
	void SetLaunchDelay(float NewDelay);

	UFUNCTION(BlueprintCallable, Category = CATEGORY_RUNTIME_DATA)
	void SetNumberOfWallParts(int32 NewCount);

	UFUNCTION(BlueprintCallable, Category = CATEGORY_RUNTIME_DATA)
	void SetRotationDuration(float NewDuration);

	UFUNCTION(BlueprintCallable, Category = CATEGORY_RUNTIME_DATA)
	void SetPlayerAimDuration(float NewDuration);

	UFUNCTION(BlueprintCallable, Category = CATEGORY_RUNTIME_DATA)
	void SetAnimationDurations(float NewYMovementDuration, float NewSpacingDuration);

	UPROPERTY(BlueprintAssignable, Category = CATEGORY_EVENTS)
	FSLOnAllWallPartsLaunched OnAllWallPartsLaunched;

protected:
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CATEGORY_COMPONENTS)
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CATEGORY_COMPONENTS)
	TArray<TObjectPtr<UStaticMeshComponent>> WallParts;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CATEGORY_COMPONENTS)
	TArray<TObjectPtr<UProjectileMovementComponent>> ProjectileMovements;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CATEGORY_COMPONENTS)
	TObjectPtr<UTimelineComponent> RotationTimeline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CATEGORY_COMPONENTS)
	TObjectPtr<UTimelineComponent> PlayerAimTimeline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CATEGORY_COMPONENTS)
	TObjectPtr<UTimelineComponent> YMovementTimeline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CATEGORY_COMPONENTS)
	TObjectPtr<UTimelineComponent> SpacingTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_RUNTIME_DATA, meta = (ClampMin = "1", ClampMax = "20"))
	int32 NumberOfWallParts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_WALL_SETTINGS)
	float WallPartSpacing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_RUNTIME_DATA)
	float LaunchDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_RUNTIME_DATA)
	float LaunchSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_WALL_SETTINGS)
	FVector WallPartScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_WALL_SETTINGS)
	bool bArrangeHorizontally;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_WALL_SETTINGS)
	float ProjectileGravityScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_ROTATION_SETTINGS)
	float RotationDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_RUNTIME_DATA, meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float RotationDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_ROTATION_SETTINGS)
	TObjectPtr<UCurveFloat> RotationCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_RUNTIME_DATA)
	float PlayerAimDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_PLAYER_AIM_SETTINGS)
	TObjectPtr<UCurveFloat> PlayerAimCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_SPAWN_ROTATION)
	FRotator SpawnRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_SPAWN_ROTATION)
	bool bApplySpawnRotationOnConstruction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_PRE_LAUNCH_ANIMATION)
	bool bEnablePreLaunchAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_PRE_LAUNCH_ANIMATION, meta = (EditCondition = "bEnablePreLaunchAnimation"))
	float YMovementDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_PRE_LAUNCH_ANIMATION, meta = (EditCondition = "bEnablePreLaunchAnimation"))
	float SpacingExpansionDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_RUNTIME_DATA, meta = (EditCondition = "bEnablePreLaunchAnimation", ClampMin = "0.1", ClampMax = "3.0"))
	float YMovementDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_RUNTIME_DATA, meta = (EditCondition = "bEnablePreLaunchAnimation", ClampMin = "0.1", ClampMax = "3.0"))
	float SpacingAnimationDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_PRE_LAUNCH_ANIMATION, meta = (EditCondition = "bEnablePreLaunchAnimation"))
	TObjectPtr<UCurveFloat> YMovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_PRE_LAUNCH_ANIMATION, meta = (EditCondition = "bEnablePreLaunchAnimation"))
	TObjectPtr<UCurveFloat> SpacingAnimationCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_EFFECTS_CHARACTER)
	TObjectPtr<UNiagaraSystem> CharacterHitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_EFFECTS_CHARACTER)
	TObjectPtr<USoundBase> CharacterHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_EFFECTS_GROUND)
	TObjectPtr<UNiagaraSystem> GroundHitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_EFFECTS_GROUND)
	TObjectPtr<USoundBase> GroundHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_MESH_SETTINGS)
	TObjectPtr<UStaticMesh> DefaultWallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_MESH_SETTINGS)
	bool bUseCustomMeshPerPart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CATEGORY_MESH_SETTINGS, meta = (EditCondition = "bUseCustomMeshPerPart"))
	TArray<TObjectPtr<UStaticMesh>> WallPartMeshes;

private:
	void CreateWallParts();
	void CreateWallPart(int32 Index);
	void UpdateWallPartsVisibility();
	UStaticMesh* GetMeshForPart(int32 Index) const;

	static const int32 MaxWallParts = 5;

	int32 CurrentLaunchIndex;
	int32 LaunchedPartsCount;
	bool bIsLaunching;
	bool bIsYMovementAnimating;
	bool bIsSpacingAnimating;
	int32 CurrentRotatingPartIndex;

	FTimerHandle LaunchTimerHandle;
	FTimerHandle RotationTimerHandle;

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
};
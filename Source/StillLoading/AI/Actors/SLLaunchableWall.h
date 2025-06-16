#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SLLaunchableWall.generated.h"

class UNiagaraSystem;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnAllWallPartsLaunched);

UENUM(BlueprintType)
enum class EWallPartState : uint8
{
	Inactive	UMETA(DisplayName = "Inactive"),
	Rotating	UMETA(DisplayName = "Rotating"),
	Launched	UMETA(DisplayName = "Launched")
};

UCLASS()
class STILLLOADING_API ASLLaunchableWall : public AActor
{
	GENERATED_BODY()
	
public:
	ASLLaunchableWall();

	UFUNCTION(BlueprintCallable, Category = "Wall")
	void LaunchWallToPlayer();

	UFUNCTION(BlueprintCallable, Category = "Wall")
	bool CanLaunch() const;

	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void RefreshWallMeshes();

	UFUNCTION(BlueprintCallable, Category = "Wall Layout")
	void RefreshWallLayout();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FSLOnAllWallPartsLaunched OnAllWallPartsLaunched;

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	void OnWallPartHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	void SetupWallParts();
	void LaunchNextPart();
	void CheckAllPartsLaunched();
	void RotateCurrentPart();
	void LaunchCurrentPart();
	void StartSmoothRotation();
	void ApplyDamageToPlayer(AActor* PlayerActor, const FHitResult& HitResult, int32 WallPartIndex);
	void PlayCharacterHitEffects(const FVector& HitLocation, AActor* HitActor);
	void PlayGroundHitEffects(const FVector& HitLocation);
	void DestroyWallPart(int32 WallPartIndex);
	FVector GetPlayerLocation() const;
	FVector GetPlayerDirection() const;
	void SetWallPartTopFaceToPlayer(UStaticMeshComponent* WallPart) const;
	
	UFUNCTION()
	void OnRotationTimelineUpdate(float Value);
	
	UFUNCTION()
	void OnRotationTimelineFinished();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TArray<TObjectPtr<UStaticMeshComponent>> WallParts;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TArray<TObjectPtr<UProjectileMovementComponent>> ProjectileMovements;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTimelineComponent> RotationTimeline;

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
	bool bApplyRotationBeforeLaunch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation Settings")
	float RotationDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation Settings", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float RotationDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation Settings")
	TObjectPtr<UCurveFloat> RotationCurve;

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

private:
	void CreateWallParts();
	void CreateWallPart(int32 Index);
	void UpdateWallPartsVisibility();
	UStaticMesh* GetMeshForPart(int32 Index) const;

	static const int32 MaxWallParts = 5;
	int32 CurrentLaunchIndex;
	int32 LaunchedPartsCount;
	bool bIsLaunching;
	FTimerHandle LaunchTimerHandle;
	FTimerHandle RotationTimerHandle;
	TArray<EWallPartState> WallPartStates;
	
	// Timeline Rotation Variables
	FOnTimelineFloat RotationTimelineUpdateDelegate;
	FOnTimelineEvent RotationTimelineFinishedDelegate;
	FRotator StartRotation;
	FRotator TargetRotation;
	int32 CurrentRotatingPartIndex;
};
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLMovementComponentBase.generated.h"

class USLInteractionComponent;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Idle UMETA(DisplayName = "Idle"),
	ECS_Cinematic UMETA(DisplayName = "Cinematic"),
	ECS_Moving UMETA(DisplayName = "Moving"),
	ECS_Jumping UMETA(DisplayName = "Jumping"),
	ECS_Falling UMETA(DisplayName = "Falling"),
	ECS_Attacking UMETA(DisplayName = "Attacking"),
	ECS_Dodging UMETA(DisplayName = "Dodging"),
	ECS_Stunned UMETA(DisplayName = "Stunned"),
	ECS_Dead UMETA(DisplayName = "Dead"),
	ECS_InputLocked UMETA(DisplayName = "Input Locked")
};

class UInputBufferComponent;
struct FInputActionValue;
enum class EInputActionType : uint8;
enum class ECharacterMontageState : uint8;
enum class EHitAnimType : uint8;

class USLSoundSubsystem;
class UCollisionRadarComponent;
class UBattleComponent;
class UCombatHandlerComponent;
class UAnimationMontageComponent;
class ASLPlayerCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Abstract)
class STILLLOADING_API USLMovementComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:
	USLMovementComponentBase();

	UFUNCTION(BlueprintCallable, Category = "Base Action")
	void OpenMenu() const;
	UFUNCTION(BlueprintCallable, Category = "Base Action")
	void Interact() const;
	UFUNCTION(BlueprintCallable, Category = "Base Action")
	void PresentGoal() const;
	UFUNCTION(BlueprintCallable, Category = "Base Action")
	void DanceTime() const;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> CachedSkeletalMesh = nullptr;

protected:
	virtual void BeginPlay() override;
	virtual void OnActionTriggered_Implementation(EInputActionType ActionType, FInputActionValue Value);
	virtual void OnActionStarted_Implementation(EInputActionType ActionType);
	virtual void OnActionCompleted_Implementation(EInputActionType ActionType);
	virtual void OnHitReceived_Implementation(AActor* Causer, float Damage, const FHitResult& HitResult, EHitAnimType AnimType);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void OnActionTriggered(EInputActionType ActionType, FInputActionValue Value);
	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void OnActionStarted(EInputActionType ActionType);
	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void OnActionCompleted(EInputActionType ActionType);
	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void OnHitReceived(AActor* Causer, float Damage, const FHitResult& HitResult, EHitAnimType AnimType);
	
	UFUNCTION()
	USLSoundSubsystem* GetBattleSoundSubSystem() const;

	UPROPERTY()
	TObjectPtr<ASLPlayerCharacter> OwnerCharacter = nullptr;
	UPROPERTY()
	TObjectPtr<UAnimationMontageComponent> CachedMontageComponent = nullptr;
	UPROPERTY()
	TObjectPtr<UCombatHandlerComponent> CachedCombatComponent = nullptr;
	UPROPERTY()
	TObjectPtr<UBattleComponent> CachedBattleComponent = nullptr;
	UPROPERTY()
	TObjectPtr<UCollisionRadarComponent> CachedRadarComponent = nullptr;
	UPROPERTY()
	TObjectPtr<UInputBufferComponent> CachedInputBufferComponent = nullptr;
	UPROPERTY()
	TObjectPtr<USLSoundSubsystem> CachedBattleSoundSubsystem = nullptr;
	UPROPERTY()
	TObjectPtr<USLInteractionComponent> CachedInteractionComponent = nullptr;
};

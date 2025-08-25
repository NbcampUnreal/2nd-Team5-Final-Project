#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "Character/BattleComponent/SLTargetableInterface.h"
#include "GameFramework/Character.h"
#include "SLMonsterAICharacterBase.generated.h"

class USLAILODComponent;
class USLAICombatComponent;
class USLAISupportModeComponent;
class UBattleComponent;
class UAnimationMontageComponent;
class USLAIAttributeComponent;
class USLAIPushComponent;
class ASLBattleManager;
class ASLSwarmSpawner;
class USLAIStateComponent;
class USLAITokenSystemComponent;

UCLASS()
class STILLLOADING_API ASLMonsterAICharacterBase : public ACharacter , public ISLTargetableInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ASLMonsterAICharacterBase();

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void SetPrimaryState(const FGameplayTag NewState);
	UFUNCTION(BlueprintCallable, Category = "State Tags")
	bool IsInPrimaryState(const FGameplayTag StateToCheck) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cached")
	TObjectPtr<USLAIStateComponent> AIStateComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cached")
	TObjectPtr<USLAICombatComponent> AICombatComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cached")
	TObjectPtr<USLAILODComponent> AILODComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cached")
	TObjectPtr<USLAIPushComponent> AIPushComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cached")
	TObjectPtr<USLAIAttributeComponent> AIAttributeComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UAnimationMontageComponent> AnimationComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UBattleComponent> BattleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cached")
	TObjectPtr<ASLSwarmSpawner> BornSpawner;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cached")
	TObjectPtr<ASLBattleManager> BattleManager;

	UPROPERTY()
	float MaxHealth;
	
	UPROPERTY()
	float CurrentHealth;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Tags")
	FGameplayTagContainer StateTags;
};

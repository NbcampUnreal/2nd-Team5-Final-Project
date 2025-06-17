#pragma once

#include "CoreMinimal.h"
#include "BattleAction.h"
#include "Components/ActorComponent.h"
#include "SLMovementComponentBase.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Abstract)
class STILLLOADING_API USLMovementComponentBase : public UActorComponent, public IBattleAction
{
	GENERATED_BODY()

public:
	USLMovementComponentBase();

	virtual void Attack_Implementation() override PURE_VIRTUAL(UBaseBattleComponent::Attack_Implementation, );

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void OpenMenu();
private:
	virtual void BeginPlay() override;
	virtual void Die();
	virtual void Interact();
};

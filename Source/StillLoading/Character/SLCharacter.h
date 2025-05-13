#pragma once

#include "CoreMinimal.h"
#include "SLBaseCharacter.h"
#include "SLCharacter.generated.h"

UCLASS()
class STILLLOADING_API ASLCharacter : public ASLBaseCharacter
{
	GENERATED_BODY()

public:
	ASLCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> SwordClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> ShieldClass;

private:
	UPROPERTY()
	TObjectPtr<AActor> Sword;
	UPROPERTY()
	TObjectPtr<AActor> Shield;

	void AttachItemToHand(AActor* ItemActor, const FName SocketName) const;
};

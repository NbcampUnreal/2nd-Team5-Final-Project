// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SLInteractableObjectBase.generated.h"

class USphereComponent;
class ASLPlayerCharacterBase;

UENUM()
enum class ESLReactiveTriggerType : uint8
{
	ERT_None,
	ERT_Hit,
	ERT_InteractKey,
	ERT_Both
};

UCLASS()
class STILLLOADING_API ASLInteractableObjectBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASLInteractableObjectBase();

	//캐릭터에서 상호작용 시 아래 함수에 접근, 상호작용 방식에 대한 값을 넣어주면 됨.
	UFUNCTION(BlueprintCallable)
	void TriggerReact(ASLPlayerCharacterBase* InCharacter, const ESLReactiveTriggerType InComingType);

	UFUNCTION(BlueprintNativeEvent)
	void OnDetected();
	virtual void OnDetected_Implementation();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnUndetected();
	virtual void OnUndetected_Implementation();
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType);
	
private:
	UFUNCTION()
	bool IsTriggerTypeAllowed(ESLReactiveTriggerType InComingType);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp = nullptr;

	UPROPERTY(EditAnywhere, Category = "Reactive")
	ESLReactiveTriggerType TriggerType = ESLReactiveTriggerType::ERT_None;

};

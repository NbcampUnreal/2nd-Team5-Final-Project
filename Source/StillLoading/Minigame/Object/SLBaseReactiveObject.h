// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLBaseReactiveObject.generated.h"

class USphereComponent;
class ASLBaseCharacter;

UENUM()
enum class ESLReactiveTriggerType : uint8
{
	ERT_None,
	ERT_Hit,
	ERT_InteractKey,
	ERT_Both
};

UCLASS()
class STILLLOADING_API ASLBaseReactiveObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASLBaseReactiveObject();

	//캐릭터에서 상호작용 시 아래 함수에 접근, 상호작용 방식에 대한 값을 넣어주면 됨.
	UFUNCTION(BlueprintCallable)
	void TriggerReact(ASLBaseCharacter* InCharacter, const ESLReactiveTriggerType InComingType);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnReacted(const ASLBaseCharacter* InCharacter);

	UFUNCTION()
	void BeginOverlapCollision(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlapCollision(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
private:
	UFUNCTION()
	bool IsTriggerTypeAllowed(ESLReactiveTriggerType InComingType);

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp = nullptr;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> CollisionComp = nullptr;

private:
	UPROPERTY(EditAnywhere, Category = "Reactive")
	ESLReactiveTriggerType TriggerType = ESLReactiveTriggerType::ERT_None;

};

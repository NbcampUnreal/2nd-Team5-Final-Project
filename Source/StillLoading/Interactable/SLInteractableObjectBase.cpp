// Fill out your copyright notice in the Description page of Project Settings.


#include "SLInteractableObjectBase.h"

#include "StillLoading\Character\SLPlayerCharacterBase.h"

ASLInteractableObjectBase::ASLInteractableObjectBase()
{
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetCollisionProfileName("RadarDetectable");
	StaticMeshComp->OnComponentBeginOverlap.AddDynamic(this, &ASLInteractableObjectBase::BeginOverlapCollision);
	StaticMeshComp->OnComponentEndOverlap.AddDynamic(this, &ASLInteractableObjectBase::EndOverlapCollision);
	SetRootComponent(StaticMeshComp);
}

void ASLInteractableObjectBase::TriggerReact(ASLPlayerCharacterBase* InCharacter, const ESLReactiveTriggerType InComingType)
{
	if (!IsValid(InCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("Character is not IsValid"));
		return;
	}
	
	if (!IsTriggerTypeAllowed(InComingType))
	{
		UE_LOG(LogTemp, Warning, TEXT("Type Failed"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("OnReacted"));
	OnInteracted(InCharacter, InComingType);
}

void ASLInteractableObjectBase::OnDetected()
{
	UE_LOG(LogTemp, Warning, TEXT("OnDetected"));
}

void ASLInteractableObjectBase::OnUndetected()
{
	UE_LOG(LogTemp, Warning, TEXT("OnUndetected"));
}


// Called when the game starts or when spawned
void ASLInteractableObjectBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASLInteractableObjectBase::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{

}

void ASLInteractableObjectBase::BeginOverlapCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if (OtherActor && OtherActor->ActorHasTag("Character"))
	//{
	//	TObjectPtr<ASLPlayerCharacterBase> Character = Cast<ASLPlayerCharacterBase>(OtherActor);
	//	if (IsValid(Character))
	//	{
	//		Character->ReactiveObject = this;
	//	}
	//}
}

void ASLInteractableObjectBase::EndOverlapCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//if (OtherActor && OtherActor->ActorHasTag("Character"))
	//{
	//	TObjectPtr<ASLPlayerCharacterBase> Character = Cast<ASLPlayerCharacterBase>(OtherActor);
	//	if (IsValid(Character) && Character->ReactiveObject == this)
	//	{
	//		Character->ReactiveObject = nullptr;
	//	}
	//}
}

bool ASLInteractableObjectBase::IsTriggerTypeAllowed(ESLReactiveTriggerType InComingType)
{
	if (TriggerType == ESLReactiveTriggerType::ERT_Both)
	{
		return true;
	}
	return InComingType == TriggerType;
}

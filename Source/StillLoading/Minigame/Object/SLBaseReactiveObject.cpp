// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLBaseReactiveObject.h"
#include "Components\SphereComponent.h"
#include "StillLoading\Character\SLPlayerCharacterBase.h"

// Sets default values
ASLBaseReactiveObject::ASLBaseReactiveObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMeshComp);
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	CollisionComp->SetupAttachment(StaticMeshComp);

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ASLBaseReactiveObject::BeginOverlapCollision);
	CollisionComp->OnComponentEndOverlap.AddDynamic(this, &ASLBaseReactiveObject::EndOverlapCollision);
	CollisionComp->SetCollisionProfileName("RadarDetectable");
	CollisionComp->SetRelativeLocation({0, 0, 32});
}

void ASLBaseReactiveObject::TriggerReact(ASLPlayerCharacterBase* InCharacter, const ESLReactiveTriggerType InComingType)
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
	OnReacted(InCharacter, InComingType);
}



// Called when the game starts or when spawned
void ASLBaseReactiveObject::BeginPlay()
{
	Super::BeginPlay();
}

void ASLBaseReactiveObject::OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{

}

void ASLBaseReactiveObject::BeginOverlapCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void ASLBaseReactiveObject::EndOverlapCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

bool ASLBaseReactiveObject::IsTriggerTypeAllowed(ESLReactiveTriggerType InComingType)
{
	if (TriggerType == ESLReactiveTriggerType::ERT_Both)
	{
		return true;
	}
	return InComingType == TriggerType;
}

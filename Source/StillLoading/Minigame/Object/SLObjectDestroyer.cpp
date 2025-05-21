// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLObjectDestroyer.h"
#include "GeometryCollection\GeometryCollectionComponent.h"

// Sets default values
ASLObjectDestroyer::ASLObjectDestroyer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMeshComponent);

	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}


// Called when the game starts or when spawned
void ASLObjectDestroyer::BeginPlay()
{
	Super::BeginPlay();
	if (StaticMeshComponent)
	{
		StaticMeshComponent->OnComponentHit.AddDynamic(this, &ASLObjectDestroyer::OnHit);
	}

	if (StaticMeshComponent && StaticMeshComponent->IsSimulatingPhysics())
	{
		FVector Direction = GetActorForwardVector(); // 자신의 정면 방향
		float ImpulseStrength = 1000.0f;

		StaticMeshComponent->AddImpulse(Direction * ImpulseStrength, NAME_None, true);
	}
	FTimerHandle DestroyTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		DestroyTimerHandle,     // 타이머 핸들
		this,                   // 대상
		&ASLObjectDestroyer::DestroySelf, // 호출할 함수
		1.0f,                   // 지연 시간 (1초)
		false                   // 반복 여부 (false면 1회만 실행)
	);
}

void ASLObjectDestroyer::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UGeometryCollectionComponent* GeoComp = Cast<UGeometryCollectionComponent>(OtherComp);
	if (GeoComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("충돌한 컴포넌트가 GeometryCollectionComponent임!"));
		OnHitDoor.Broadcast();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("충돌한 컴포넌트가 GeometryCollectionComponent 아님!"));
	}
	
}


void ASLObjectDestroyer::DestroySelf()
{
	Destroy();
}



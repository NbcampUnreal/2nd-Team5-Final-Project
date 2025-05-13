#include "SLDefaultSword.h"

#include "Character/BattleComponent/BattleComponent.h"
#include "Components/BoxComponent.h"

ASLDefaultSword::ASLDefaultSword()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionSphere"));
	BoxComponent->SetupAttachment(ItemMesh);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ASLDefaultSword::OnBoxOverlap);

	BoxComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASLDefaultSword::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASLDefaultSword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASLDefaultSword::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Sword Overlapped with: %s"), *OtherActor->GetName());

		if (UBattleComponent* BattleComponent = FindComponentByClass<UBattleComponent>())
		{
			
		}
	}
}

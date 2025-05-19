#include "SLItem.h"

ASLItem::ASLItem()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(SceneComponent);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(SceneComponent);
}

void ASLItem::BeginPlay()
{
	Super::BeginPlay();
}

void ASLItem::BindOverlap(UPrimitiveComponent* OverlapComponent)
{
	if (OverlapComponent) {
		// 오버랩 연결시에 사용, 지금은 스윕으로 사용
		//OverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &ASLItem::HandleOverlap);
	}
}

void ASLItem::HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner() || HitActors.Contains(OtherActor)) return;

	UE_LOG(LogTemp, Log, TEXT("[%s] Overlapped with: %s"), *GetName(), *OtherActor->GetName());
	
	HitActors.Add(OtherActor);
	OnItemOverlap.Broadcast(OtherActor);
}


#include "SLItem.h"


ASLItem::ASLItem()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(SceneComponent);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(SceneComponent);
}

void ASLItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASLItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


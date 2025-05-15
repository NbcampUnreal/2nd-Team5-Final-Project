#include "ChargingWidgetActor.h"

#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"

AChargingWidgetActor::AChargingWidgetActor()
{
	PrimaryActorTick.bCanEverTick = true;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	RootComponent = WidgetComponent;

	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
	WidgetComponent->SetDrawSize(FVector2D(150.f, 150.f));
	WidgetComponent->SetTwoSided(true);
}

void AChargingWidgetActor::BeginPlay()
{
	Super::BeginPlay();
}

void AChargingWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetCharacter && TargetCharacter->GetMesh())
	{
		const FVector SocketLocation = TargetCharacter->GetMesh()->GetSocketLocation(SocketName);
		SetActorLocation(SocketLocation + Offset);
	}
}

void AChargingWidgetActor::SetTargetCharacter(ACharacter* Target)
{
	TargetCharacter = Target;
}


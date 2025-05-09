#include "SLBaseCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ASLBaseCharacter::ASLBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 500.0f;
	SpringArmComponent->bUsePawnControlRotation = true;

	DefaultCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	DefaultCamera->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	DefaultCamera->bUsePawnControlRotation = false;
}

void ASLBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASLBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


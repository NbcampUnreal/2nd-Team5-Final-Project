#include "SLBaseCharacter.h"

#include "BattleComponent/BattleComponent.h"
#include "Camera/CameraComponent.h"
#include "CameraManagerComponent/CameraManagerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ASLBaseCharacter::ASLBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	//GetCharacterMovement()->bOrientRotationToMovement = true; // Zelda-like
	//GetCharacterMovement()->RotationRate = FRotator(0.f, 80.f, 0.f);
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true; // 자체 회전 제어
	//CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 3.f;

	DefaultCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DefaultCamera"));
	DefaultCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	BattleCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("BattleCamera"));
	BattleCamera->SetupAttachment(RootComponent);

	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCamera->SetupAttachment(RootComponent);

	SideViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCamera->SetupAttachment(RootComponent);

	CameraManager = CreateDefaultSubobject<UCameraManagerComponent>(TEXT("CameraManager"));
}

void ASLBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CameraManager)
	{
		CameraManager->SetCameraRefs(DefaultCamera, BattleCamera, TopDownCamera, SideViewCamera);
	}
}

void ASLBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ASLBaseCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (UBattleComponent* BattleComp = FindComponentByClass<UBattleComponent>())
	{
		ISLBattleInterface::Execute_ReceiveBattleDamage(BattleComp, ActualDamage);
	}

	return ActualDamage;
}


#include "SLBaseCharacter.h"

#include "BattleComponent/BattleComponent.h"
#include "Buffer/InputBufferComponent.h"
#include "Camera/CameraComponent.h"
#include "CameraManagerComponent/CameraManagerComponent.h"
#include "DynamicIMCComponent/SLDynamicIMCComponent.h"
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

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom);

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(RootComponent);

	SideViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCamera->SetupAttachment(RootComponent);

	// 컴포넌트 연결
	CameraManager = CreateDefaultSubobject<UCameraManagerComponent>(TEXT("CameraManager"));

	if (UInputBufferComponent* BufferComp = FindComponentByClass<UInputBufferComponent>())
	{
		if (UDynamicIMCComponent* IMCComp = FindComponentByClass<UDynamicIMCComponent>())
		{
			IMCComp->OnActionStarted.AddDynamic(BufferComp, &UInputBufferComponent::OnIMCActionStarted);
		}
	}
}

void ASLBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CameraManager)
	{
		CameraManager->SetCameraRefsofCharacter(ThirdPersonCamera, FirstPersonCamera, SideViewCamera);
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


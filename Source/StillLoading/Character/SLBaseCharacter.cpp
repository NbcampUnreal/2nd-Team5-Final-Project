#include "SLBaseCharacter.h"

#include "Camera/CameraComponent.h"
#include "CameraManagerComponent/CameraManagerComponent.h"
#include "GameFramework/SpringArmComponent.h"

ASLBaseCharacter::ASLBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("DefaultSpringArm"));
	DefaultSpringArm->SetupAttachment(RootComponent);
	DefaultSpringArm->TargetArmLength = 500.f;
	DefaultSpringArm->bUsePawnControlRotation = true;

	DefaultCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DefaultCamera"));
	DefaultCamera->SetupAttachment(DefaultSpringArm);

	BattleSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("BattleSpringArm"));
	BattleSpringArm->SetupAttachment(RootComponent);
	BattleSpringArm->TargetArmLength = 400.f;

	BattleCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("BattleCamera"));
	BattleCamera->SetupAttachment(BattleSpringArm);

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


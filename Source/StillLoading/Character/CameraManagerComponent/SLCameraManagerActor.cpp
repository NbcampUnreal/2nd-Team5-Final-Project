// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CameraManagerComponent/SLCameraManagerActor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Character/CameraManagerComponent/CameraManagerComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ASLCameraManagerActor::ASLCameraManagerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCamera->SetupAttachment(Root);
	TopDownCamera->SetRelativeLocation(FVector(0.f, 0.f, 600.f));
	TopDownCamera->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	TopDownCamera->bAutoActivate = true;
	TopDownCamera->ProjectionMode = ECameraProjectionMode::Orthographic;

	CinematicCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CinematicCamera"));
	CinematicCamera->SetupAttachment(Root);
	CinematicCamera->SetRelativeLocation(FVector(300.f, 0.f, 500.f));
	CinematicCamera->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	CinematicCamera->bAutoActivate = false;

}

void ASLCameraManagerActor::SetCameraMode(EGameCameraType Mode)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	/*APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
		UCameraManagerComponent* CMC = GetOwner()->FindComponentByClass<UCameraManagerComponent>();
		if(CMC)
		{
			CurrentCameraMode = CMC->GetCurrnetCameraMode();
		}
	}*/

	CurrentCameraMode = Mode;

	switch (Mode)
	{
	case EGameCameraType::EGCT_TopDown:
		TopDownCamera->SetActive(true);
		CinematicCamera->SetActive(false);
		PC->SetViewTargetWithBlend(this, 0.5f);
		break;

	case EGameCameraType::EGCT_Cinematic:
		CinematicCamera->SetActive(true);
		TopDownCamera->SetActive(false);
		PC->SetViewTargetWithBlend(this, 1.0f);
		break;
	}
}

// Called when the game starts or when spawned
void ASLCameraManagerActor::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void ASLCameraManagerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TargetFollow(DeltaTime);
}

void ASLCameraManagerActor::TargetFollow(float DeltaTime)
{
	
	if (CurrentCameraMode == EGameCameraType::EGCT_TopDown)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (PlayerPawn)
		{
			FVector TargetLocation = PlayerPawn->GetActorLocation();
			TargetLocation.Z += 600.0f; // 고도 유지
			SetActorLocation(FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, 10.f));
		}
	}
}


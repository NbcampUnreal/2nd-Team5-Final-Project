// Fill out your copyright notice in the Description page of Project Settings.
#include "SLPlayerRidingCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "StillLoading/Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Character/FlashComponent/SLFlashComponent.h"
#include "Character/PlayerState/SLBattlePlayerState.h"


ASLPlayerRidingCharacter::ASLPlayerRidingCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    FlashComponent = CreateDefaultSubobject<USLFlashComponent>(TEXT("Flash"));
    
    DynamicIMCComponent = CreateDefaultSubobject<UDynamicIMCComponent>(TEXT("DynamicIMCComponent"));
    
    GetCharacterMovement()->bOrientRotationToMovement = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
}

void ASLPlayerRidingCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (DynamicIMCComponent)
    {
        DynamicIMCComponent->OnActionTriggered.AddDynamic(this, &ASLPlayerRidingCharacter::OnActionTriggeredCallback);
        DynamicIMCComponent->OnActionStarted.AddDynamic(this, &ASLPlayerRidingCharacter::OnActionStartedCallback);
        DynamicIMCComponent->OnActionCompleted.AddDynamic(this, &ASLPlayerRidingCharacter::OnActionCompletedCallback);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicIMCComponent is nullptr on %s"), *GetName());
    }
    
    CurrentHealth = MaxHealth;
}

void ASLPlayerRidingCharacter::OnActionTriggeredCallback(const EInputActionType ActionType, const FInputActionValue InputValue)
{
    switch (ActionType)
    {
        case EInputActionType::EIAT_MoveUp:
            AddMovementInput(FVector::ForwardVector, InputValue.Get<float>());
            break;
        case EInputActionType::EIAT_MoveDown:
            AddMovementInput(-(FVector::ForwardVector), InputValue.Get<float>());
            break;
        case EInputActionType::EIAT_MoveLeft:
            AddMovementInput(-(FVector::RightVector), InputValue.Get<float>());
            break;
        case EInputActionType::EIAT_MoveRight:
            AddMovementInput(FVector::RightVector, InputValue.Get<float>());
            break;
        default:
            break;
    }
}

void ASLPlayerRidingCharacter::OnActionStartedCallback(const EInputActionType ActionType)
{
    switch (ActionType)
    {
    case EInputActionType::EIAT_Jump:
        break;
    default:
        break;
    }
}

void ASLPlayerRidingCharacter::OnActionCompletedCallback(const EInputActionType ActionType)
{
    switch (ActionType)
    {
    case EInputActionType::EIAT_Jump:
        break;
    default:
        break;
    }
}

float ASLPlayerRidingCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (FlashComponent->IsFlashing()) return 0.f;

    ASLBattlePlayerState* SLPlayerState = Cast<ASLBattlePlayerState>(GetPlayerState());

    if (IsValid(SLPlayerState))
    {
        SLPlayerState->DecreaseHealth(DamageAmount);
    }

    //CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC && PC->PlayerCameraManager)
    {
        PC->PlayerCameraManager->StartCameraShake(CameraShakeClass);
    }
    if (FlashComponent)
    {
        FlashComponent->StartFlashing();
    }
    /*if (CurrentHealth <= 0.0f)
    {
        UE_LOG(LogTemp, Error, TEXT("Player Died!"));
    }*/
    return 0.f;
}
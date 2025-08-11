// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SLPlayerRunnerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimInstance.h"
#include "Character\Animation/SLRunnerAnimInstance.h"
#include "Minigame/Object/SLRunnerHurdle.h"
#include "StillLoading/Character/DynamicIMCComponent/SLDynamicIMCComponent.h"


ASLPlayerRunnerCharacter::ASLPlayerRunnerCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    // IMC
    DynamicIMCComponent = CreateDefaultSubobject<UDynamicIMCComponent>(TEXT("DynamicIMCComponent"));

    // 충돌 박스(플레이어 기준)
    BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
    BoxComp->SetupAttachment(GetRootComponent());
    BoxComp->InitBoxExtent(FVector(30.f, 40.f, 90.f));
    BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    BoxComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
    BoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    BoxComp->SetGenerateOverlapEvents(true);

    // 이동 회전 고정(러너 스타일)
    GetCharacterMovement()->bOrientRotationToMovement = false;
    bUseControllerRotationYaw   = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll  = false;

    MaxHealth = 2;
    CurrentHealth = MaxHealth;
}

void ASLPlayerRunnerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (DynamicIMCComponent)
    {
        // 네 컴포넌트의 델리게이트 시그니처에 맞춰 연결
        DynamicIMCComponent->OnActionTriggered.AddDynamic(this, &ASLPlayerRunnerCharacter::OnActionTriggeredCallback);
    }

    if (IsValid(BoxComp))
    {
        BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ASLPlayerRunnerCharacter::OnOverlapBegin);
    }
}

USLRunnerAnimInstance* ASLPlayerRunnerCharacter::GetRunnerAnim() const
{
    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        return Cast<USLRunnerAnimInstance>(MeshComp->GetAnimInstance());
    }
    return nullptr;
}

void ASLPlayerRunnerCharacter::PushToAnim(ERunnerAction Action) const
{
    if (auto* R = GetRunnerAnim())
    {
        R->PushAction(Action);
    }
}

void ASLPlayerRunnerCharacter::PlayMatched(EHurdleState State) const
{
    if (auto* R = GetRunnerAnim())
    {
        R->PlayMatchedMontage(State);
    }
}

void ASLPlayerRunnerCharacter::OnActionTriggeredCallback(const EInputActionType ActionType, const FInputActionValue InputValue)
{
    // Shift=Slide, Space=Jump, 좌클릭=Attack (확정)
    switch (ActionType)
    {
    case EInputActionType::EIAT_MoveLeft:
        AddMovementInput(-FVector::RightVector, InputValue.Get<float>());
        break;

    case EInputActionType::EIAT_MoveRight:
        AddMovementInput(FVector::RightVector, InputValue.Get<float>());
        break;

    case EInputActionType::EIAT_Jump:
        CurrentState = EHurdleState::Jump;
        PushToAnim(ERunnerAction::Jump);
        GetWorldTimerManager().SetTimer(
            StateTimerHandle, [this]{ CurrentState = EHurdleState::None; }, 0.5f, false);
        break;

    case EInputActionType::EIAT_Attack:
        CurrentState = EHurdleState::Attack;
        PushToAnim(ERunnerAction::Attack);
        GetWorldTimerManager().SetTimer(
            StateTimerHandle, [this]{ CurrentState = EHurdleState::None; }, 0.5f, false);
        break;

    case EInputActionType::EIAT_Special:
        CurrentState = EHurdleState::Sliding;
        PushToAnim(ERunnerAction::Slide);
        GetWorldTimerManager().SetTimer(
            StateTimerHandle, [this]{ CurrentState = EHurdleState::None; }, 0.5f, false);
        break;

    default:
        break;
    }
}

void ASLPlayerRunnerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor) return;
    if (!OtherActor->ActorHasTag(TEXT("Hurdle"))) return;

    if (ASLRunnerHurdle* Hurdle = Cast<ASLRunnerHurdle>(OtherActor))
    {
        const EHurdleState ObState = Hurdle->GetHurdleState();

        // 상태 일치 + None 아님 -> 회피 성공
        if (ObState == CurrentState && CurrentState != EHurdleState::None)
        {
            PlayMatched(ObState);

            // 짧은 무적(I-Frame)로 연속 판정 방지
            if (!bInvincible)
            {
                bInvincible = true;
                GetWorldTimerManager().SetTimer(
                    IFrameTimerHandle, [this]{ bInvincible = false; }, IFrameDuration, false);
            }
        }
        else
        {
            // 불일치 -> 피해
            if (!bInvincible)
            {
                ApplyDamage();
            }
        }
    }
}

void ASLPlayerRunnerCharacter::ApplyDamage()
{
    CurrentHealth -= 1;
    if (CurrentHealth <= 0)
    {
        OnDie();
        return;
    }

    // 체력 자동 회복(연습용)
    GetWorldTimerManager().SetTimer(
        RecoveryTimerHandle,
        [this]{ CurrentHealth = MaxHealth; },
        RecoveryTime, false);
}

void ASLPlayerRunnerCharacter::OnDie()
{
    // TODO: 사망 처리(리트라이/리셋 등)
    DisableInput(nullptr);
    // 필요 시 애님 몽타주/카메라 연출 추가
}
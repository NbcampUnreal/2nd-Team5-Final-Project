#include "SLNPCCharacter.h"

#include "NiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AISense_Damage.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/SLBaseAIController.h"
#include "Components/CapsuleComponent.h"

ASLNPCCharacter::ASLNPCCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    
    DefaultKnockbackForce = 500.0f;
    KnockbackUpwardForce = 200.0f;
    
    MaxHealth = 5.0f;
    CurrentHealth = MaxHealth;
    
    bCanBeExecuted = false;
    bIsHitReaction = true;
}

void ASLNPCCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    CurrentHealth = MaxHealth;
}

void ASLNPCCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ASLNPCCharacter::CharacterHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType AnimType)
{
    if (bIsDead || bIsInvincibility)
    {
        return;
    }
    
    // 데미지 적용
    SetCurrentHealth(CurrentHealth - DamageAmount);

    // 데미지 감지 이벤트 보고
    if (DamageCauser)
    {
        UAISense_Damage::ReportDamageEvent(
            GetWorld(),
            this,
            DamageCauser,
            DamageAmount,
            GetActorLocation(),
            HitResult.Location
        );
    }

    // 사망 처리
    if (CurrentHealth <= 0.0f)
    {
        ProcessDeath();
        return;
    }

    // 넉백 적용
    if (DamageCauser)
    {
        ApplyKnockback(DamageCauser, DefaultKnockbackForce);
    }

    // 기존 히트 반응 처리 (방향 계산 및 상태 설정)
    if (DamageCauser && ShouldPlayHitReaction(DamageAmount))
    {
        FVector AttackerLocation = DamageCauser->GetActorLocation();
        FVector DirectionVector = AttackerLocation - GetActorLocation();
        DirectionVector.Normalize();
        
        FVector LocalHitDirection = GetActorTransform().InverseTransformVectorNoScale(DirectionVector);

        float AbsX = FMath::Abs(LocalHitDirection.X);
        float AbsY = FMath::Abs(LocalHitDirection.Y);

        if (AbsY > AbsX)
        {
            HitDirection = (LocalHitDirection.Y > 0) ? EHitDirection::EHD_Right : EHitDirection::EHD_Left;
        }
        else 
        {
            HitDirection = (LocalHitDirection.X > 0) ? EHitDirection::EHD_Front : EHitDirection::EHD_Back;
        }

        HitDirectionVector = LocalHitDirection;
        SetHitState(true, 5.0f);
    }

    // 히트 이펙트 재생
    if (HitEffectComponent)
    {
        FVector EffectLocation = HitResult.bBlockingHit ? HitResult.ImpactPoint : HitResult.Location;
        
        if (EffectLocation.IsZero())
        {
            EffectLocation = GetActorLocation();
            EffectLocation.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.5f;
        }
        
        HitEffectComponent->SetWorldLocation(EffectLocation);
        HitEffectComponent->ActivateSystem();
    }
}

void ASLNPCCharacter::SetHitState(bool bNewIsHit, float AutoResetTime)
{
    // 기존 히트 상태 타이머가 있다면 정리
    if (HitStateResetTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(HitStateResetTimer);
    }
    
    bIsHit = bNewIsHit;
    
    // AI Controller의 블랙보드에도 동기화
    if (AIController)
    {
        if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
        {
            BlackboardComponent->SetValueAsBool(FName("IsHit"), bIsHit);
        }
    }
    
    // 새로운 히트 상태일 때만 자동 리셋 타이머 설정
    if (bNewIsHit && AutoResetTime > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(HitStateResetTimer, [this]()
        {
            bIsHit = false;
            
            // AI Controller 블랙보드도 업데이트
            if (AIController)
            {
                if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
                {
                    BlackboardComponent->SetValueAsBool(FName("IsHit"), false);
                }
            }
        }, AutoResetTime, false);
    }
}

void ASLNPCCharacter::ApplyKnockback(AActor* DamageCauser, float KnockbackForce)
{
    if (!DamageCauser || !GetCharacterMovement())
    {
        return;
    }

    // 넉백 방향 계산 (공격자 -> NPC 반대 방향)
    FVector KnockbackDirection = GetActorLocation() - DamageCauser->GetActorLocation();
    KnockbackDirection.Z = 0.0f; // 수평 방향으로만
    KnockbackDirection.Normalize();

    // 넉백 벡터 생성
    FVector LaunchVelocity = KnockbackDirection * KnockbackForce;
    LaunchVelocity.Z = KnockbackUpwardForce; // 약간의 상승력 추가

    // 캐릭터 발사
    LaunchCharacter(LaunchVelocity, true, false);
}
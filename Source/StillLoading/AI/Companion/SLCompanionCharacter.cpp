#include "SLCompanionCharacter.h"

#include "NiagaraComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AnimInstances/SLAICharacterAnimInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "AI/GamePlayTag/AIGamePlayTag.h"
#include "AI/Projectile/SLAIProjectile.h"
#include "AI/Projectile/SLHomingProjectile.h"
#include "AI/SLMonster/SLMonster.h"
#include "Components/CapsuleComponent.h"
#include "Controller/SLBaseAIController.h"
#include "Controller/SLCompanionAIController.h"

ASLCompanionCharacter::ASLCompanionCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // 기본 설정
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    
    // 무적 설정
    bCanBeExecuted = false;
    IsHitReaction = true;
    
    // 전투 설정
    ProjectileSocketName = "hand_r";
    AutoStunInterval = 10.0f;
    StunDuration = 3.0f;
    bIsInCombat = false;
    IsBattleMage = false;
}

void ASLCompanionCharacter::BeginPlay()
{
    Super::BeginPlay();
    
}

void ASLCompanionCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
}

void ASLCompanionCharacter::SetCombatMode(bool bInCombat)
{
    bIsInCombat = bInCombat;
    
    if (AIController)
    {
        if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
        {
            BlackboardComponent->SetValueAsBool(FName("IsInCombat"), bIsInCombat);
        }

        ASLCompanionAIController* CompanionAIController = Cast<ASLCompanionAIController>(AIController);
        if (CompanionAIController)
        {
            if (bIsInCombat)
            {
                CompanionAIController->UpdateSightRadius(1000.0f, 1200.0f);
            }
            else
            {
                CompanionAIController->UpdateSightRadius(200.0f, 300.0f);
            }
        }
    }
}

bool ASLCompanionCharacter::HasGameplayTag(const FGameplayTag& TagToCheck) const
{
    return CurrentGameplayTags.HasTag(TagToCheck);
}

void ASLCompanionCharacter::FireProjectile(EAttackAnimType AttackAnimType)
{
    if (!ProjectileClass || !AIController) return;
    
    // 타겟 가져오기
    UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
    if (!BlackboardComponent) return;
    
    AActor* Target = Cast<AActor>(BlackboardComponent->GetValueAsObject(FName("TargetActor")));
    if (!Target) return;
    
    // 발사 위치 계산
    FVector SpawnLocation = GetMesh()->GetSocketLocation(ProjectileSocketName);
    FRotator SpawnRotation = (Target->GetActorLocation() - SpawnLocation).Rotation();
    
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = this;
    
    if (ASLHomingProjectile* Projectile = GetWorld()->SpawnActor<ASLHomingProjectile>(
        ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams))
    {
        Projectile->SetHomingTarget(Target);
        Projectile->SetupSpawnedProjectile(AttackAnimType, 1500.0f);
    }
}

bool ASLCompanionCharacter::GetIsBattleMage()
{
    return IsBattleMage;
}

void ASLCompanionCharacter::SetIsBattleMage(bool bInBattleMage)
{
    IsBattleMage = bInBattleMage;
}

void ASLCompanionCharacter::CastStunSpell(AActor* Target)
{
    if (!Target) return;
    
    // 몬스터인지 확인
    ASLAIBaseCharacter* EnemyCharacter = Cast<ASLAIBaseCharacter>(Target);
    if (!EnemyCharacter || EnemyCharacter->GetIsDead()) return;
    
    // 상태이상 효과 적용
    if (USLAICharacterAnimInstance* AnimInstance = Cast<USLAICharacterAnimInstance>(EnemyCharacter->GetMesh()->GetAnimInstance()))
    {
        //AnimInstance->SetIsStun(true);
    }
    
    // 상태이상 이펙트 재생
    if (StunEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            StunEffect,
            Target->GetActorLocation(),
            FRotator::ZeroRotator,
            FVector(1.0f),
            true,
            true,
            ENCPoolMethod::AutoRelease
        );
    }
    
    // 기절 해제 타이머
    FTimerHandle StunTimer;
    GetWorld()->GetTimerManager().SetTimer(StunTimer, [EnemyCharacter, this]()
    {
        if (IsValid(EnemyCharacter) && !EnemyCharacter->GetIsDead())
        {
            if (USLAICharacterAnimInstance* AnimInstance = Cast<USLAICharacterAnimInstance>(EnemyCharacter->GetMesh()->GetAnimInstance()))
            {
                AnimInstance->SetIsStun(false);
            }
        }
    }, StunDuration, false);
}


ECompanionActionPattern ASLCompanionCharacter::SelectRandomPattern(const TArray<ECompanionActionPattern>& Patterns)
{
    int32 RandomIndex = FMath::RandRange(0, Patterns.Num() - 1);
    return Patterns[RandomIndex];
}

void ASLCompanionCharacter::AutoStunNearbyEnemy()
{
    if (!bIsInCombat || !AIController) return;
    
    // 현재 타겟 가져오기
    UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
    if (!BlackboardComponent) return;
    
    AActor* Target = Cast<AActor>(BlackboardComponent->GetValueAsObject(FName("TargetActor")));
    if (Target)
    {
        CastStunSpell(Target);
    }
}

void ASLCompanionCharacter::CharacterHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EAttackAnimType AnimType)
{
    if (DamageCauser && IsHitReaction && AnimInstancePtr)
    {
        // 히트 반응 애니메이션 설정
        if (USLAICharacterAnimInstance* SLAIAnimInstance = Cast<USLAICharacterAnimInstance>(AnimInstancePtr.Get()))
        {
            // 히트 방향 계산
            FVector AttackerLocation = DamageCauser->GetActorLocation();
            FVector DirectionVector = AttackerLocation - GetActorLocation();
            DirectionVector.Normalize();
            
            FVector LocalHitDirection = GetActorTransform().InverseTransformVectorNoScale(DirectionVector);

            // 히트 방향 결정
            EHitDirection HitDir;
            float AbsX = FMath::Abs(LocalHitDirection.X);
            float AbsY = FMath::Abs(LocalHitDirection.Y);

            if (AbsY > AbsX)
            {
                HitDir = (LocalHitDirection.Y > 0) ? EHitDirection::EHD_Right : EHitDirection::EHD_Left;
            }
            else 
            {
                HitDir = (LocalHitDirection.X > 0) ? EHitDirection::EHD_Front : EHitDirection::EHD_Back;
            }
            
            // 애니메이션 인스턴스에 히트 정보 설정
            SLAIAnimInstance->SetHitDirection(HitDir);
            SLAIAnimInstance->SetIsHit(true);
        }
    }

    if (HitEffectComponent)
    {
        // ImpactPoint가 유효하면 사용, 아니면 Location 사용
        FVector EffectLocation = HitResult.bBlockingHit ? HitResult.ImpactPoint : HitResult.Location;
        
        // 만약 둘 다 Zero 벡터라면 캐릭터 중심 사용
        if (EffectLocation.IsZero())
        {
            EffectLocation = GetActorLocation();
            EffectLocation.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.5f;
        }
        
        HitEffectComponent->SetWorldLocation(EffectLocation);
        HitEffectComponent->ActivateSystem();
    }
}
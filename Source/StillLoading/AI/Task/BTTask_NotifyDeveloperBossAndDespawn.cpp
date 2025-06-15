#include "BTTask_NotifyDeveloperBossAndDespawn.h"
#include "Character/SLAIBaseCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "AI/DeveloperBoss/SLDeveloperBoss.h"
#include "AI/Doppelganger/SLDoppelgangerCharacter.h"
#include "Character/Item/SLItem.h"

UBTTask_NotifyDeveloperBossAndDespawn::UBTTask_NotifyDeveloperBossAndDespawn()
{
    NodeName = "Notify Developer Boss And Despawn";
    DespawnDelay = 0.5f;
    bPlayDeathAnimation = false;
}

EBTNodeResult::Type UBTTask_NotifyDeveloperBossAndDespawn::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("BTTask_NotifyDeveloperBossAndDespawn: No AI Controller"));
        return EBTNodeResult::Failed;
    }
    
    ASLAIBaseCharacter* AICharacter = Cast<ASLAIBaseCharacter>(AIController->GetPawn());
    if (!AICharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("BTTask_NotifyDeveloperBossAndDespawn: No AI Character"));
        return EBTNodeResult::Failed;
    }
    
    // 개발자 보스 찾기
    ASLDeveloperBoss* DeveloperBoss = FindDeveloperBoss();
    if (!DeveloperBoss)
    {
        UE_LOG(LogTemp, Warning, TEXT("BTTask_NotifyDeveloperBossAndDespawn: No Developer Boss found in level"));
        // 개발자 보스가 없어도 캐릭터는 제거
        DespawnCharacter(AICharacter);
        return EBTNodeResult::Succeeded;
    }
    
    // 패턴 완료 알림
    AICharacter->NotifyPatternFinished();
    
    UE_LOG(LogTemp, Display, TEXT("BTTask: %s notified pattern finished to Developer Boss"), *AICharacter->GetName());
    
    // 디스폰 처리
    if (DespawnDelay > 0.0f)
    {
        // 지연 후 디스폰
        FTimerHandle DespawnTimer;
        FTimerDelegate TimerDelegate;
        TimerDelegate.BindLambda([this, AICharacter]()
        {
            DespawnCharacter(AICharacter);
        });
        
        AICharacter->GetWorld()->GetTimerManager().SetTimer(DespawnTimer, TimerDelegate, DespawnDelay, false);
    }
    else
    {
        // 즉시 디스폰
        DespawnCharacter(AICharacter);
    }
    
    return EBTNodeResult::Succeeded;
}

FString UBTTask_NotifyDeveloperBossAndDespawn::GetStaticDescription() const
{
    return FString::Printf(TEXT("Notify Developer Boss and despawn after %.1f seconds\nPlay Death Animation: %s"), 
        DespawnDelay, 
        bPlayDeathAnimation ? TEXT("Yes") : TEXT("No"));
}

ASLDeveloperBoss* UBTTask_NotifyDeveloperBossAndDespawn::FindDeveloperBoss() const
{
    UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        return nullptr;
    }
    
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(World, ASLDeveloperBoss::StaticClass(), FoundActors);
    
    if (FoundActors.Num() > 0)
    {
        return Cast<ASLDeveloperBoss>(FoundActors[0]);
    }
    
    return nullptr;
}

void UBTTask_NotifyDeveloperBossAndDespawn::DespawnCharacter(ASLAIBaseCharacter* Character)
{
    if (!IsValid(Character))
    {
        return;
    }
    
    // 캐릭터가 가진 무기 파괴
    DestroyCharacterWeapons(Character);
    
    if (bPlayDeathAnimation && !Character->GetIsDead())
    {
        // 죽음 애니메이션 재생
        Character->HandleDeath();
        
        // 애니메이션 후 제거
        FTimerHandle DestroyTimer;
        Character->GetWorld()->GetTimerManager().SetTimer(DestroyTimer, [Character]()
        {
            if (IsValid(Character))
            {
                Character->Destroy();
            }
        }, 3.0f, false);
    }
    else
    {
        // 즉시 제거
        Character->Destroy();
    }
    
    UE_LOG(LogTemp, Display, TEXT("BTTask: %s despawned with weapons"), *Character->GetName());
}

void UBTTask_NotifyDeveloperBossAndDespawn::DestroyCharacterWeapons(ASLAIBaseCharacter* Character)
{
    if (!Character)
    {
        return;
    }
    
    // 기본 장착 무기 파괴
    if (Character->GetEquippedWeapon())
    {
        Character->GetEquippedWeapon()->Destroy();
        UE_LOG(LogTemp, Display, TEXT("Destroyed equipped weapon"));
    }
    
    // 도플갱어의 경우 추가 무기 파괴
    if (ASLDoppelgangerCharacter* Doppelganger = Cast<ASLDoppelgangerCharacter>(Character))
    {
        if (Doppelganger->GetSword())
        {
            Doppelganger->GetSword()->Destroy();
            UE_LOG(LogTemp, Display, TEXT("Destroyed Doppelganger's sword"));
        }
        
        if (Doppelganger->GetShield())
        {
            Doppelganger->GetShield()->Destroy();
            UE_LOG(LogTemp, Display, TEXT("Destroyed Doppelganger's shield"));
        }
    }
}
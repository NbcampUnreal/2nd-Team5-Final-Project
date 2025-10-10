#include "SLStableMasterAIController.h"

#include "AI/AIInterface/SLHideableInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Character/SLPlayerCharacterBase.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogStableMasterAI, Log, All);

ASLStableMasterAIController::ASLStableMasterAIController()
{
    SetAITeamId(FGenericTeamId(1));
    
    AISenseConfig_Hearing = CreateDefaultSubobject<UAISenseConfig_Hearing>("AISenseConfig_Hearing");
    AISenseConfig_Hearing->HearingRange = 3000.0f;
    AISenseConfig_Hearing->DetectionByAffiliation.bDetectEnemies = true;
    AISenseConfig_Hearing->DetectionByAffiliation.bDetectFriendlies = false;
    AISenseConfig_Hearing->DetectionByAffiliation.bDetectNeutrals = true;
    
    if (AIPerceptionComponent)
    {
       AIPerceptionComponent->ConfigureSense(*AISenseConfig_Hearing);
    }

    CurrentlyWatchingPlayer = nullptr;
}

void ASLStableMasterAIController::Tick(float DeltaTime)
{
    // 현재 타겟이 숨어있는 상태라면 부모 Tick 스킵
    if (CurrentBestTarget.Get() && CurrentBestTarget->GetClass()->ImplementsInterface(USLHideableInterface::StaticClass()))
    {
        bool bIsHiding = ISLHideableInterface::Execute_IsHiding(CurrentBestTarget.Get());
        if (bIsHiding)
        {
            return;
        }
    }
    
    Super::Tick(DeltaTime);
}

void ASLStableMasterAIController::BeginPlay()
{
    Super::BeginPlay();
    
    if (AIPerceptionComponent && AISenseConfig_Hearing)
    {
       AIPerceptionComponent->ConfigureSense(*AISenseConfig_Hearing);
    }
}

void ASLStableMasterAIController::OnAIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    // 타겟을 잃었을 때 처리
    if (!Stimulus.WasSuccessfullySensed() && Actor == CurrentBestTarget)
    {
        HandleTargetLost(Actor);
        return;
    }

    // 숨기 상태 체크
    if (Actor && Actor->GetClass()->ImplementsInterface(USLHideableInterface::StaticClass()))
    {
        bool bIsHiding = ISLHideableInterface::Execute_IsHiding(Actor);
        if (bIsHiding)
        {
            if (ASLPlayerCharacterBase* PlayerBase = Cast<ASLPlayerCharacterBase>(Actor))
            {
                if (PlayerBase->StimuliSource)
                {
                    PlayerBase->StimuliSource->SetActive(false);
                }
            }
            
            HandleHidingPlayer(Actor);
            return; 
        }
    }

    // 자극 타입별 처리
    if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
    {
       HandleHearingStimulus(Actor, Stimulus);
       return; // 소리 자극은 부모 클래스 호출하지 않음
    }
    else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
    {
       HandleSightStimulus(Actor, Stimulus);
    }
    
    // 시각 자극만 부모 클래스 호출 (타겟 설정)
    Super::OnAIPerceptionUpdated(Actor, Stimulus);
}

void ASLStableMasterAIController::OnTargetPerceptionForgotten(AActor* Actor)
{
    UE_LOG(LogStableMasterAI, Warning, TEXT("Target forgotten: %s"), *Actor->GetName());
    
    RemoveTarget(Actor);
    
    if (CurrentBestTarget == Actor)
    {
        UE_LOG(LogStableMasterAI, Warning, TEXT("Current target forgotten - immediate clear without re-evaluation"));
        ClearTargetAndSetInvestigation(Actor, EInvestigateType::Seen);
    }
    
    // 감시 상태 해제
    SetWatchingState(Actor, false);
}

void ASLStableMasterAIController::HandleTargetLost(AActor* Actor)
{
    SetWatchingState(Actor, false);
    ClearTargetAndSetInvestigation(Actor, EInvestigateType::Seen);
    RemoveTarget(Actor);
}

void ASLStableMasterAIController::HandleHidingPlayer(AActor* Actor)
{
    RemoveTarget(Actor);
    SetWatchingState(Actor, false);
    
    if (CurrentBestTarget == Actor)
    {
        CurrentBestTarget = nullptr;
        if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
        {
            BlackboardComponent->ClearValue(FName("TargetActor"));
            BlackboardComponent->ClearValue(FName("InvestigateLocation"));
            BlackboardComponent->SetValueAsBool(FName("ShouldInvestigate"), false);
        }
    }
}

void ASLStableMasterAIController::HandleHearingStimulus(AActor* Actor, const FAIStimulus& Stimulus)
{
    if (!Stimulus.WasSuccessfullySensed())
    {
        return;
    }
    
    if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
    {
        // 소리가 발생한 위치 저장 (오브젝트 위치)
        BlackboardComponent->SetValueAsVector(FName("SoundLocation"), Stimulus.StimulusLocation);
        BlackboardComponent->SetValueAsFloat(FName("SoundTime"), GetWorld()->GetTimeSeconds());
       
        // 현재 타겟이 없을 때만 소리 발생 위치를 조사하도록 설정
        AActor* CurrentTarget = Cast<AActor>(BlackboardComponent->GetValueAsObject(FName("TargetActor")));
        if (!CurrentTarget)
        {
            // 플레이어를 타겟으로 설정하는 것이 아니라, 소리 발생 위치를 조사하도록 설정
            SetInvestigationLocation(Stimulus.StimulusLocation, EInvestigateType::Sound);
        }
    }
}

void ASLStableMasterAIController::HandleSightStimulus(AActor* Actor, const FAIStimulus& Stimulus)
{
    if (!Actor || !Actor->GetClass()->ImplementsInterface(USLHideableInterface::StaticClass()))
    {
       return;
    }

    if (Stimulus.WasSuccessfullySensed())
    {
        SetWatchingState(Actor, true);
    }
    else
    {
        SetWatchingState(Actor, false);
    }
}

void ASLStableMasterAIController::ClearTargetAndSetInvestigation(AActor* Actor, EInvestigateType InvestigateType)
{
    if (!Actor)
    {
        return;
    }

    FVector LastSeenLocation = Actor->GetActorLocation();
    CurrentBestTarget = nullptr;
    
    if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
    {
        BlackboardComponent->ClearValue(FName("TargetActor"));
        BlackboardComponent->SetValueAsVector(FName("LastSeenTargetLocation"), LastSeenLocation);
        
        SetInvestigationLocation(LastSeenLocation, InvestigateType);
    }
}

void ASLStableMasterAIController::SetWatchingState(AActor* Actor, bool bIsWatching)
{
    if (!Actor || !Actor->GetClass()->ImplementsInterface(USLHideableInterface::StaticClass()))
    {
        return;
    }

    if (bIsWatching)
    {
        if (CurrentlyWatchingPlayer != Actor)
        {
            CurrentlyWatchingPlayer = Actor;
            ISLHideableInterface::Execute_SetBeingWatched(Actor, true);
        }
    }
    else
    {
        if (CurrentlyWatchingPlayer == Actor)
        {
            CurrentlyWatchingPlayer = nullptr;
            ISLHideableInterface::Execute_SetBeingWatched(Actor, false);
        }
    }
}

void ASLStableMasterAIController::SetInvestigationLocation(const FVector& Location, EInvestigateType InvestigateType)
{
    if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
    {
        BlackboardComponent->SetValueAsVector(FName("InvestigateLocation"), Location);
        BlackboardComponent->SetValueAsBool(FName("ShouldInvestigate"), true);
        BlackboardComponent->SetValueAsEnum(FName("InvestigateType"), static_cast<uint8>(InvestigateType));
    }
}
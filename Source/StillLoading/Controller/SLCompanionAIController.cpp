// Fill out your copyright notice in the Description page of Project Settings.


#include "SLCompanionAIController.h"

#include "EngineUtils.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/SLPlayerCharacter.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"


ASLCompanionAIController::ASLCompanionAIController()
{
    // 플레이어와 같은 팀 (TeamId = 0)
    SetGenericTeamId(FGenericTeamId(0));
    
    // 시야 설정
    AISenseConfig_Sight->SightRadius = 200.f;
    AISenseConfig_Sight->LoseSightRadius = 300.f;
    AISenseConfig_Sight->PeripheralVisionAngleDegrees = 180.f;
    
    // 다른 AI에게 적대적이지 않음
    bIsHostileToOtherAI = false;
}

void ASLCompanionAIController::BeginPlay()
{
    Super::BeginPlay();
    
    // 플레이어 찾기
    if (!PlayerToFollow)
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            // 플레이어 컨트롤러로부터 캐릭터 가져오기
            ASLPlayerCharacter* PlayerCharacter = Cast<ASLPlayerCharacter>(PC->GetPawn());
            if (PlayerCharacter)
            {
                PlayerToFollow = PlayerCharacter;
            }
        }
    }
}

void ASLCompanionAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
    {
        // 플레이어 설정
        if (PlayerToFollow)
        {
            BlackboardComponent->SetValueAsObject(FName("PlayerToFollow"), PlayerToFollow);
        }
        
        // 기본 상태 설정
        BlackboardComponent->SetValueAsBool(FName("IsInCombat"), false);
        BlackboardComponent->SetValueAsFloat(FName("FollowDistance"), 300.0f);
    }
}

void ASLCompanionAIController::SetPlayerToFollow(ACharacter* PlayerCharacter)
{
    PlayerToFollow = PlayerCharacter;
    
    if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
    {
        BlackboardComponent->SetValueAsObject(FName("PlayerToFollow"), PlayerCharacter);
    }
}

ETeamAttitude::Type ASLCompanionAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
    const APawn* OtherPawn = Cast<const APawn>(&Other);
    if (!OtherPawn)
    {
        return ETeamAttitude::Neutral;
    }
    
    const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController());
    if (!OtherTeamAgent)
    {
        return ETeamAttitude::Neutral;
    }
    
    FGenericTeamId OtherTeamID = OtherTeamAgent->GetGenericTeamId();
    FGenericTeamId MyTeamID = GetGenericTeamId();
    
    // 같은 팀 (플레이어 팀)
    if (OtherTeamID == 0 || OtherTeamID == MyTeamID)
    {
        return ETeamAttitude::Friendly;
    }
    // 적 팀
    else if (OtherTeamID == 2)
    {
        return ETeamAttitude::Hostile;
    }
    
    return ETeamAttitude::Neutral;
}

void ASLCompanionAIController::OnAIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
    {
        // 전투 모드에서만 타겟 설정
        bool bIsInCombat = BlackboardComponent->GetValueAsBool(FName("IsInCombat"));
        
        if (bIsInCombat && Stimulus.WasSuccessfullySensed() && Actor)
        {
            if (GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile)
            {
                
                // 현재 타겟이 없거나 새 타겟이 더 가까운 경우
                AActor* CurrentTarget = Cast<AActor>(BlackboardComponent->GetValueAsObject(FName("TargetActor")));
                
                if (!CurrentTarget)
                {
                    BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
                }
                else
                {
                    float DistanceToCurrent = FVector::Dist(GetPawn()->GetActorLocation(), CurrentTarget->GetActorLocation());
                    float DistanceToNew = FVector::Dist(GetPawn()->GetActorLocation(), Actor->GetActorLocation());
                    
                    if (DistanceToNew < DistanceToCurrent)
                    {
                        BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
                    }
                }
            }
        }
    }
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/SLMonsterAIController.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/Character.h"

ASLMonsterAIController::ASLMonsterAIController()
{
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    SetPerceptionComponent(*AIPerceptionComponent);

    UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 2000.f;
    SightConfig->LoseSightRadius = 1200.f;
    SightConfig->PeripheralVisionAngleDegrees = 150.f;
    SightConfig->SetMaxAge(5.f);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;

    AIPerceptionComponent->ConfigureSense(*SightConfig);
    AIPerceptionComponent->SetDominantSense(*SightConfig->GetSenseImplementation());
}

void ASLMonsterAIController::BeginPlay()
{
    Super::BeginPlay();

    AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ASLMonsterAIController::OnTargetPerceived);
}

void ASLMonsterAIController::OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus)
{
    if (ACharacter* SensedCharacter = Cast<ACharacter>(Actor))
    {
        //if (SensedCharacter->IsPlayerControlled())
        //{
            if (Stimulus.WasSuccessfullySensed())
            {
                GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), Actor);
            }
            else
            {
                GetBlackboardComponent()->ClearValue(FName("TargetActor"));
            }
        //}
    }
}


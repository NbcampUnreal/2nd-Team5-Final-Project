#include "BTDecorator_CheckDistanceToHome.h"

#include "AI/RealAI/Blackboardkeys.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_CheckDistanceToHome::UBTDecorator_CheckDistanceToHome()
{
    NodeName = TEXT("Check Distance To Home");

    HomeLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_CheckDistanceToHome, HomeLocationKey));
    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_CheckDistanceToHome, TargetActorKey), AActor::StaticClass());
    AvailDistanceKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_CheckDistanceToHome, AvailDistanceKey));
}

bool UBTDecorator_CheckDistanceToHome::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

    const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return false;
    }

    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(BlackboardKeys::TargetActor));
    if (!TargetActor)
    {
        return false;
    }

    const FVector HomeLocation = BlackboardComp->GetValueAsVector(BlackboardKeys::HomeLocation);
    const float AvailDistance = BlackboardComp->GetValueAsFloat(BlackboardKeys::AvailDistance);
    const FVector TargetLocation = TargetActor->GetActorLocation();

    const float DistanceToHomeSquared = FVector::DistSquared(HomeLocation, TargetLocation);
    const float AvailDistanceSquared = AvailDistance * AvailDistance;

    bResult = (DistanceToHomeSquared <= AvailDistanceSquared);
    if (bResult)
    {
        UE_LOG(LogTemp, Warning, TEXT("Distance Avail"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Distance Fail"));
    }
    return bResult;
}

FString UBTDecorator_CheckDistanceToHome::GetStaticDescription() const
{
    return FString::Printf(TEXT("Is Target '%s' within '%s' distance from Home '%s'?"),
        *TargetActorKey.SelectedKeyName.ToString(),
        *AvailDistanceKey.SelectedKeyName.ToString(),
        *HomeLocationKey.SelectedKeyName.ToString()
    );
}
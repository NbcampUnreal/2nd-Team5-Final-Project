

#include "SLReactiveObjectBrokenSpline.h"


ASLReactiveObjectBrokenSpline::ASLReactiveObjectBrokenSpline()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASLReactiveObjectBrokenSpline::OnReacted(const ASLPlayerCharacterBase* InCharacter,ESLReactiveTriggerType InTriggerType)
{
    Super::OnReacted(InCharacter, InTriggerType);

    OnBroken();
}



#include "SLAIBaseCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/SLBaseAIController.h"


ASLAIBaseCharacter::ASLAIBaseCharacter()
{
}

void ASLAIBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<ASLBaseAIController>(GetController());
    
	if (AIController)
	{        
		BlackboardComp = AIController->GetBlackboardComponent();
		UE_LOG(LogTemp, Warning, TEXT("블랙보드 초기화. The Name is : %s"), *BlackboardComp->GetName());        
	}
}



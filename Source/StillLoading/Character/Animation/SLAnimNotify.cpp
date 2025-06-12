#include "SLAnimNotify.h"

#include "Character/SLPlayerCharacter.h"
#include "Character/MovementHandlerComponent/SL25DMovementHandlerComponent.h"
#include "Character/MovementHandlerComponent/SL2DMovementHandlerComponent.h"
#include "Character/MovementHandlerComponent/SLMovementHandlerComponent.h"

void USLAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                           const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	ASLPlayerCharacter* MyCharacter = Cast<ASLPlayerCharacter>(Owner);
	if (!MyCharacter) return;

	UMovementHandlerComponent* ActionComp = MyCharacter->FindComponentByClass<UMovementHandlerComponent>();
	if (ActionComp) ActionComp->OnAttackStageFinished(CharacterMontageStage);
	
	USL2DMovementHandlerComponent* Action2DComp = MyCharacter->FindComponentByClass<USL2DMovementHandlerComponent>();
	if (Action2DComp) Action2DComp->OnAttackStageFinished(CharacterMontageStage);

	USL25DMovementHandlerComponent* Action25DComp = MyCharacter->FindComponentByClass<USL25DMovementHandlerComponent>();
	if (Action25DComp) Action25DComp->OnAttackStageFinished(CharacterMontageStage);
}
#include "AnimNotify_SLCharacter.h"

#include "Character/SLCharacter.h"
#include "Character/MovementHandlerComponent/SLMovementHandlerComponent.h"

void USLAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                           const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	ASLCharacter* MyCharacter = Cast<ASLCharacter>(Owner);
	if (!MyCharacter) return;

	UMovementHandlerComponent* ActionComp = MyCharacter->FindComponentByClass<UMovementHandlerComponent>();
	if (!ActionComp) return;

	ActionComp->OnAttackStageFinished(CharacterMontageStage);
}

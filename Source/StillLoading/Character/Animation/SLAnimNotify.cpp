#include "SLAnimNotify.h"

#include "Character/SLPlayerCharacter.h"
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
	if (!ActionComp) return;

	ActionComp->OnAttackStageFinished(CharacterMontageStage);
}
#include "SLMonsterHitRecoveryNotify.h"

#include "AI/RealAI/MonsterAICharacter.h"

void USLMonsterHitRecoveryNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	AMonsterAICharacter* MyCharacter = Cast<AMonsterAICharacter>(Owner);
	if (!MyCharacter) return;

	MyCharacter->HandleHitNotify();
	
}

#include "SLMonsterAnimNotify.h"

#include "AI/RealAI/SLMonsterAICharacter.h"

void USLMonsterAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                  const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(Owner);
	if (!MyCharacter) return;

	MyCharacter->HandleAnimNotify(MonsterMontageStage);
}

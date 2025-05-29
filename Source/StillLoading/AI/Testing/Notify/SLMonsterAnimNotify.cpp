#include "SLMonsterAnimNotify.h"

#include "AI/Testing/MonsterAICharacter.h"

void USLMonsterAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                  const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	AMonsterAICharacter* MyCharacter = Cast<AMonsterAICharacter>(Owner);
	if (!MyCharacter) return;

	MyCharacter->HandleAnimNotify(MonsterMontageStage);
}

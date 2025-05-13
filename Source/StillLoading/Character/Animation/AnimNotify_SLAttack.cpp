#include "AnimNotify_SLAttack.h"

void USLAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	UE_LOG(LogTemp, Warning, TEXT("USLAnimNotify::Montage Notify"));
}

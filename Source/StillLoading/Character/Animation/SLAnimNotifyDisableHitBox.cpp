#include "SLAnimNotifyDisableHitBox.h"

#include "Character/SLPlayerCharacter.h"
#include "Character/Interface/OverlapToggleable.h"

void USLAnimNotifyDisableHitBox::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	const ASLPlayerCharacter* Owner = Cast<ASLPlayerCharacter>(MeshComp->GetOwner());

	if (!Owner) return;

	switch (TargetItemType) {
	case EItemType::IT_Sword:
		if (IOverlapToggleable* Weapon = Cast<IOverlapToggleable>(Owner->Sword))
		{
			Weapon->DisableOverlap();
		}
		break;
	case EItemType::IT_Shield:
		if (IOverlapToggleable* Weapon = Cast<IOverlapToggleable>(Owner->Shield))
		{
			Weapon->DisableOverlap();
		}
		break;
	}
}

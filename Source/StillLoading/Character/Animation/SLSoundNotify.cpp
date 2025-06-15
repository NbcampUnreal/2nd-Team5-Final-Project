#include "SLSoundNotify.h"

#include "Kismet/GameplayStatics.h"
#include "SubSystem/SLSoundSubsystem.h"

void USLSoundNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                            const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	const UWorld* World = MeshComp->GetWorld();
    
	if (World && World->IsGameWorld())
	{
		if (const AActor* Owner = MeshComp->GetOwner())
		{
			if (USLSoundSubsystem* BattleSoundSubsystem = UGameplayStatics::GetGameInstance(MeshComp)->GetSubsystem<USLSoundSubsystem>())
			{
				BattleSoundSubsystem->PlayBattleSound(SoundType, Owner->GetActorLocation());
			}
		}
	}
}

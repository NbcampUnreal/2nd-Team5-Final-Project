#include "SLCameraShakeNotify.h"
#include "Kismet/GameplayStatics.h"

void USLCameraShakeNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                  const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!CameraShakeClass || !MeshComp) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(MeshComp->GetWorld(), 0);
	if (PC && PC->PlayerCameraManager)
	{
		PC->PlayerCameraManager->StartCameraShake(CameraShakeClass);
	}
}


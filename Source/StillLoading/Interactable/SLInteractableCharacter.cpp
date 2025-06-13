#include "SLInteractableCharacter.h"

#include "SLTalkHandlerBase.h"
#include "UI/SLUISubsystem.h"


ASLInteractableCharacter::ASLInteractableCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	CharacterMesh->SetupAttachment(RootComponent);
	CharacterMesh->SetCollisionProfileName("RadarDetectable");
	TargetName = "NPC";
}

void ASLInteractableCharacter::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	if (IsValid(CurrentTalkHandler))
	{
		if (USLBaseTextPrintWidget* TextWidget = UISubsystem->ActivateTalk(ESLTalkTargetType::ETT_NPC, TargetName, CurrentTalkHandler->GetTalkName()))
		{
			TextWidget->OnTalkEnded.AddUniqueDynamic(this, &ASLInteractableCharacter::OnCurrentTalkEnd);
			CurrentTextWidget = TextWidget;
		}
	}
}

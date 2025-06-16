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
	if (USLTalkHandlerBase* TalkHandler = GetCurrentTalkHandler())
	{
		if (USLBaseTextPrintWidget* TextWidget = UISubsystem->GetTalkWidget())
		{
			TextWidget->OnTalkEnded.AddUniqueDynamic(this, &ThisClass::OnCurrentTalkEnd);
			TextWidget->OnChoiceEnded.AddUniqueDynamic(this, &ThisClass::OnCurrentChoiceEnd);
			UISubsystem->ActivateTalk(ESLTalkTargetType::ETT_NPC, TargetName, TalkHandler->GetTalkName());
		}
	}
}

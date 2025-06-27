#include "SLInteractableCharacter.h"

#include "SLTalkHandlerBase.h"
#include "Components/BoxComponent.h"
#include "UI/SLUISubsystem.h"


ASLInteractableCharacter::ASLInteractableCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	CharacterMesh->SetupAttachment(RootComponent);
	CharacterMesh->SetCollisionProfileName("BlockAllDynamic");
	TargetName = "NPC";

	InteractionCollision->SetBoxExtent({50,50,100});
	InteractionCollision->SetRelativeLocation({0,0,50});
}

void ASLInteractableCharacter::StartTalk()
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
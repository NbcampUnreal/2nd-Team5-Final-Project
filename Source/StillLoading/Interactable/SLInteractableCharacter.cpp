#include "SLInteractableCharacter.h"

#include "SLTalkHandlerBase.h"
#include "UI/SLUISubsystem.h"


ASLInteractableCharacter::ASLInteractableCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	CharacterMesh->SetupAttachment(RootComponent);
	CharacterMesh->SetCollisionProfileName("RadarDetectable");

	BaseTalkHandler = CreateDefaultSubobject<USLTalkHandlerBase>(TEXT("SLTalkHandlerBase"));
	TriggerType = ESLReactiveTriggerType::ERT_InteractKey;
}

void ASLInteractableCharacter::SetCurrentTalkHandler(USLTalkHandlerBase* TalkHandler)
{
	CurrentTalkHandler = TalkHandler;
}

void ASLInteractableCharacter::OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	if (CurrentTalkHandler.IsValid())
	{
		if (USLBaseTextPrintWidget* TextWidget = UISubsystem->ActivateTalk(ESLTalkTargetType::ETT_NPC, TargetName, CurrentTalkHandler->GetTalkName()))
		{
			TextWidget->OnTalkEnded.AddUniqueDynamic(this, &ASLInteractableCharacter::OnCurrentTalkEnd);
		}
	}
}

void ASLInteractableCharacter::OnCurrentTalkEnd()
{
	if (CurrentTalkHandler.IsValid())
	{
		CurrentTalkHandler->OnTalkEnd();
	}
}

void ASLInteractableCharacter::BeginPlay()
{
	Super::BeginPlay();
	UISubsystem = GetGameInstance()->GetSubsystem<USLUISubsystem>();
	check(UISubsystem)
	
	CurrentTalkHandler = BaseTalkHandler;
}

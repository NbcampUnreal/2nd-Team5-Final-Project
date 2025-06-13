#include "SLInteractableCharacter.h"

#include "SLTalkHandlerBase.h"
#include "UI/SLUISubsystem.h"


ASLInteractableCharacter::ASLInteractableCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	CharacterMesh->SetupAttachment(RootComponent);

	BaseTalkHandler = CreateDefaultSubobject<USLTalkHandlerBase>(TEXT("Base Talk Handler"));
	CurrentTalkHandler = BaseTalkHandler;
}

void ASLInteractableCharacter::SetCurrentTalkHandler(USLTalkHandlerBase* TalkHandler)
{
	CurrentTalkHandler = TalkHandler;
}

void ASLInteractableCharacter::OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	if (CurrentTalkHandler.IsValid())
	{
		//auto& [OnTalkEnded] = UISubsystem->ActivateTalk(ESLTalkTargetType::ETT_NPC, TargetName, CurrentTalkHandler->GetTalkName());
		//OnTalkEnded.AddDynamic(this, &ASLInteractableCharacter::OnCurrentTalkEnd);
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
}

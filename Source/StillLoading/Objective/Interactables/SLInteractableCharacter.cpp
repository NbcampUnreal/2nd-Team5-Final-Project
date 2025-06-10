#include "SLInteractableCharacter.h"

#include "UI/SLUISubsystem.h"

ASLInteractableCharacter::ASLInteractableCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	CharacterMesh->SetupAttachment(RootComponent);
}

void ASLInteractableCharacter::Interaction()
{
	ActivateCurrentTalk();
}

void ASLInteractableCharacter::BeginPlay()
{
	Super::BeginPlay();
	UISubsystem = GetGameInstance()->GetSubsystem<USLUISubsystem>();
	check(UISubsystem)
}

void ASLInteractableCharacter::ActivateCurrentTalk()
{
	UISubsystem->ActivateTalk(ESLTalkTargetType::ETT_NPC, CurrentTargetName, CurrentTalkName);
}


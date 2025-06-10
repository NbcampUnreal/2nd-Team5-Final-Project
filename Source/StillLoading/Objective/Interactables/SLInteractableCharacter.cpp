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
	UISubsystem->ActivateTalk(ESLTalkTargetType::ETT_NPC, CurrentTargetName, CurrentTalkNames[CurrentTalkIndex]);
}

void ASLInteractableCharacter::BeginPlay()
{
	Super::BeginPlay();
	UISubsystem = GetGameInstance()->GetSubsystem<USLUISubsystem>();
	check(UISubsystem)
}

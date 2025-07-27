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

	TalkTargetType = ESLTalkTargetType::ETT_NPC;
}
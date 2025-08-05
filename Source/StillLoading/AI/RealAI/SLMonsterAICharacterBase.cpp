#include "SLMonsterAICharacterBase.h"

#include "Character/BattleComponent/BattleComponent.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "Component/AIAttributeComponent.h"
#include "Component/SLAICombatComponent.h"
#include "Component/SLAIPushComponent.h"
#include "Component/SLAIStateComponent.h"

ASLMonsterAICharacterBase::ASLMonsterAICharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	AIStateComp = CreateDefaultSubobject<USLAIStateComponent>(TEXT("AIStateComponent"));
	AICombatComp = CreateDefaultSubobject<USLAICombatComponent>(TEXT("AICombatComponent"));
	AIPushComp = CreateDefaultSubobject<USLAIPushComponent>(TEXT("AIPushComponent"));
	AIAttributeComp = CreateDefaultSubobject<UAIAttributeComponent>(TEXT("AIAttributeComponent"));
	AnimationComponent = CreateDefaultSubobject<UAnimationMontageComponent>(TEXT("AnimationComponent"));
	BattleComponent = CreateDefaultSubobject<UBattleComponent>(TEXT("BattleComponent"));
}

void ASLMonsterAICharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASLMonsterAICharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASLMonsterAICharacterBase::SetPrimaryState(const FGameplayTag NewState)
{
	StateTags.Reset();
	StateTags.AddTag(NewState);
}

bool ASLMonsterAICharacterBase::IsInPrimaryState(const FGameplayTag StateToCheck) const
{
	return StateTags.HasTagExact(StateToCheck);
}


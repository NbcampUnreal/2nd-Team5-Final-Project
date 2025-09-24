#include "SLMonsterAICharacterBase.h"

#include "Character/BattleComponent/BattleComponent.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "Component/SLAIAttributeComponent.h"
#include "Component/SLAICombatComponent.h"
#include "Component/SLAILODComponent.h"
#include "Component/SLAIStateComponent.h"

ASLMonsterAICharacterBase::ASLMonsterAICharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	AIStateComp = CreateDefaultSubobject<USLAIStateComponent>(TEXT("AIStateComponent"));
	AICombatComp = CreateDefaultSubobject<USLAICombatComponent>(TEXT("AICombatComponent"));
	AILODComp = CreateDefaultSubobject<USLAILODComponent>(TEXT("AILODComponent"));
	AIAttributeComp = CreateDefaultSubobject<USLAIAttributeComponent>(TEXT("AIAttributeComponent"));
	AnimationComponent = CreateDefaultSubobject<UAnimationMontageComponent>(TEXT("AnimationComponent"));
	BattleComponent = CreateDefaultSubobject<UBattleComponent>(TEXT("BattleComponent"));
}

void ASLMonsterAICharacterBase::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	this->TeamId = NewTeamID;
}

FGenericTeamId ASLMonsterAICharacterBase::GetGenericTeamId() const
{
	return this->TeamId;
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


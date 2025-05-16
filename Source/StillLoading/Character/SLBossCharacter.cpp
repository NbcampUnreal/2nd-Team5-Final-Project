// Fill out your copyright notice in the Description page of Project Settings.


#include "SLBossCharacter.h"

#include "AnimInstances/SLAICharacterAnimInstance.h"
#include "AnimInstances/SLBossAnimInstance.h"


ASLBossCharacter::ASLBossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	BossAttackPattern = EBossAttackPattern::EBAP_None;
}

void ASLBossCharacter::SetBossAttackPattern(EBossAttackPattern NewPattern)
{
	BossAttackPattern = NewPattern;

	if (USLBossAnimInstance* SLAIAnimInstance = Cast<USLBossAnimInstance>(AnimInstancePtr.Get()))
	{
		SLAIAnimInstance->SetBossAttackPattern(NewPattern);
	}
}

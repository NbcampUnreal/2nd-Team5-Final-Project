// Fill out your copyright notice in the Description page of Project Settings.


#include "SLBossAnimInstance.h"

void USLBossAnimInstance::SetBossAttackPattern(EBossAttackPattern NewBossAttackPattern)
{
	BossAttackPattern = NewBossAttackPattern;
}

EBossAttackPattern USLBossAnimInstance::GetBossAttackPattern()
{
	return BossAttackPattern;
}

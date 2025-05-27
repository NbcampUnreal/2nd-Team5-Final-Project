// Fill out your copyright notice in the Description page of Project Settings.


#include "SLCompanionAnimInstance.h"

USLCompanionAnimInstance::USLCompanionAnimInstance()
{
}

ECompanionActionPattern USLCompanionAnimInstance::GetCompanionPattern() const
{
	return CompanionPattern;
}

void USLCompanionAnimInstance::SetCompanionPattern(const ECompanionActionPattern NewCompanionPattern)
{
	CompanionPattern = NewCompanionPattern;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "SLTalkHandlerBase.h"

USLTalkHandlerBase::USLTalkHandlerBase()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLTalkHandlerBase::OnTalkEnd_Implementation()
{
	OnTalkEndDelegate.Broadcast(this);
}

void USLTalkHandlerBase::OnChoiceEnd_Implementation(bool bResult)
{
	OnChoiceEndDelegate.Broadcast(bResult);
}

FName USLTalkHandlerBase::GetTalkName()
{
	return TalkName;
}

void USLTalkHandlerBase::SetTalkName(const FName InName)
{
	TalkName = InName;
}

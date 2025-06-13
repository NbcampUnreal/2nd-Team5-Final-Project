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

FName USLTalkHandlerBase::GetTalkName()
{
	return TalkName;
}

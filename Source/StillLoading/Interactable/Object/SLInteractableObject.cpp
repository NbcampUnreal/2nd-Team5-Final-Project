// Fill out your copyright notice in the Description page of Project Settings.


#include "SLInteractableObject.h"

#include "Interactable/SLTalkHandlerBase.h"
#include "UI/SLUISubsystem.h"
#include "UI/SLUITypes.h"
#include "UI/Widget/AdditiveWidget/SLBaseTextPrintWidget.h"


ASLInteractableObject::ASLInteractableObject()
{
	PrimaryActorTick.bCanEverTick = false;

	BaseTalkHandler = CreateDefaultSubobject<USLTalkHandlerBase>(TEXT("SLTalkHandlerBase"));
	TriggerType = ESLReactiveTriggerType::ERT_InteractKey;
	TargetName = "Object";
	CurrentTalkHandler= BaseTalkHandler;
}

void ASLInteractableObject::SetCurrentTalkHandler(USLTalkHandlerBase* TalkHandler)
{
	CurrentTalkHandler = TalkHandler;
}

void ASLInteractableObject::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	if (IsValid(CurrentTalkHandler))
	{
		if (USLBaseTextPrintWidget* TextWidget = UISubsystem->ActivateTalk(ESLTalkTargetType::ETT_Object, TargetName, CurrentTalkHandler->GetTalkName()))
		{
			TextWidget->OnTalkEnded.AddUniqueDynamic(this, &ASLInteractableObject::OnCurrentTalkEnd);
			CurrentTextWidget = TextWidget;
		}
	}
}

void ASLInteractableObject::OnCurrentTalkEnd()
{
	if (IsValid(CurrentTalkHandler) && IsValid(CurrentTextWidget))
	{
		CurrentTalkHandler->OnTalkEnd();
		CurrentTextWidget->OnTalkEnded.RemoveDynamic(this, &ASLInteractableObject::OnCurrentTalkEnd);
	}
}

void ASLInteractableObject::BeginPlay()
{
	Super::BeginPlay();
	UISubsystem = GetGameInstance()->GetSubsystem<USLUISubsystem>();
	check(UISubsystem)
}

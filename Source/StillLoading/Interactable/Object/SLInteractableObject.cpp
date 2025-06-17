// Fill out your copyright notice in the Description page of Project Settings.


#include "SLInteractableObject.h"

#include "Interactable/SLTalkHandlerBase.h"
#include "UI/SLUISubsystem.h"
#include "UI/SLUITypes.h"
#include "UI/Widget/AdditiveWidget/SLBaseTextPrintWidget.h"


ASLInteractableObject::ASLInteractableObject()
{
	PrimaryActorTick.bCanEverTick = false;

	DefaultTalkHandler = CreateDefaultSubobject<USLTalkHandlerBase>(TEXT("기본 대화 핸들러"));
	TriggerType = ESLReactiveTriggerType::ERT_InteractKey;
	TargetName = "Object";
	CurrentTalkHandler = DefaultTalkHandler;
}

void ASLInteractableObject::SetCurrentTalkHandler(USLTalkHandlerBase* TalkHandler)
{
	CurrentTalkHandler = TalkHandler;
}

USLTalkHandlerBase* ASLInteractableObject::GetCurrentTalkHandler()
{
	if (CurrentTalkHandler.IsValid())
	{
		return CurrentTalkHandler.Get();
	}
	CurrentTalkHandler = DefaultTalkHandler;
	return DefaultTalkHandler;
}

void ASLInteractableObject::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	if (USLTalkHandlerBase* TalkHandler = GetCurrentTalkHandler())
	{
		if (USLBaseTextPrintWidget* TextWidget = UISubsystem->GetTalkWidget())
		{
			TextWidget->OnTalkEnded.AddUniqueDynamic(this, &ThisClass::OnCurrentTalkEnd);
			TextWidget->OnChoiceEnded.AddUniqueDynamic(this, &ThisClass::OnCurrentChoiceEnd);
			UISubsystem->ActivateTalk(ESLTalkTargetType::ETT_Object, TargetName, TalkHandler->GetTalkName());
		}
	}
}

void ASLInteractableObject::OnCurrentTalkEnd()
{
	USLTalkHandlerBase* TalkHandler = GetCurrentTalkHandler();
	USLBaseTextPrintWidget* TextWidget = UISubsystem->GetTalkWidget();
	if (TalkHandler && TextWidget)
	{
		TalkHandler->OnTalkEnd();
		TextWidget->OnTalkEnded.RemoveDynamic(this, &ThisClass::OnCurrentTalkEnd);
	}
}

void ASLInteractableObject::OnCurrentChoiceEnd(bool bResult)
{
	USLTalkHandlerBase* TalkHandler = GetCurrentTalkHandler();
	USLBaseTextPrintWidget* TextWidget = UISubsystem->GetTalkWidget();
	if (TalkHandler && TextWidget)
	{
		TalkHandler->OnChoiceEnd(bResult);
		TextWidget->OnChoiceEnded.RemoveDynamic(this, &ThisClass::OnCurrentChoiceEnd);
	}
}

void ASLInteractableObject::BeginPlay()
{
	Super::BeginPlay();
	UISubsystem = GetGameInstance()->GetSubsystem<USLUISubsystem>();
	check(UISubsystem)
}

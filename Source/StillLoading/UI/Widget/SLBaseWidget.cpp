// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SLBaseWidget.h"
#include "UI/SLUISubsystem.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "Animation/WidgetAnimation.h"

void USLBaseWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	UISubsystem = NewUISubsystem;

	if (IsValid(OpenAnim))
	{
		EndOpenAnimDelegate.BindDynamic(this, &ThisClass::OnEndedOpenAnim);
		BindToAnimationFinished(OpenAnim, EndOpenAnimDelegate);
	}

	if (IsValid(CloseAnim))
	{
		EndCloseAnimDelegate.BindDynamic(this, &ThisClass::OnEndedCloseAnim);
		BindToAnimationFinished(CloseAnim, EndCloseAnimDelegate);
	}

	ApplyOnChangedChapter(ChapterType);
	
	CheckValidOfTextPoolSubsystem();
	TextPoolSubsystem->LanguageDelegate.AddDynamic(this, &ThisClass::NotifyChangedLanguage);

	ApplyTextData();
}

void USLBaseWidget::ApplyOnChangedChapter(ESLChapterType ChapterType)
{
	CurrentChapter = ChapterType;
	FindWidgetData();

	ApplyImageData();
	ApplyFontData();
}

ESLInputModeType USLBaseWidget::GetWidgetInputMode() const
{
	return WidgetInputMode;
}

int32 USLBaseWidget::GetWidgetOrder() const
{
	return WidgetOrder;
}

bool USLBaseWidget::GetWidgetCursorMode() const
{
	return bIsVisibleCursor;
}

void USLBaseWidget::ActivateWidget(ESLChapterType ChapterType)
{
	if (CurrentChapter != ChapterType)
	{
		ApplyOnChangedChapter(ChapterType);
	}
}

void USLBaseWidget::OnEndedCloseAnim()
{
	this->RemoveFromViewport();
}

void USLBaseWidget::NotifyChangedLanguage()
{
	ApplyTextData();
}

void USLBaseWidget::PlayUISound(ESLUISoundType SoundType)
{
	CheckValidOfUISubsystem();
	UISubsystem->PlayUISound(SoundType);
}

void USLBaseWidget::CheckValidOfUISubsystem()
{
	if (IsValid(UISubsystem))
	{
		return;
	}

	checkf(IsValid(GetGameInstance()), TEXT("GameInstance is invalid"));
	UISubsystem = GetGameInstance()->GetSubsystem<USLUISubsystem>();
	checkf(IsValid(UISubsystem), TEXT("UISubsystem is invalid"));
}

void USLBaseWidget::CheckValidOfTextPoolSubsystem()
{
	if (IsValid(TextPoolSubsystem))
	{
		return;
	}

	TextPoolSubsystem = GetGameInstance()->GetSubsystem<USLTextPoolSubsystem>();
	checkf(IsValid(TextPoolSubsystem), TEXT("TextPool Subsystem is invalid"));
}

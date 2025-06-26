// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/SLTextPoolSettings.h"
#include "SubSystem/SLLevelTransferSubsystem.h"

void USLTextPoolSubsystem::OnChangedLanguage(ESLLanguageType LanguageType)
{
	CurrentLanguage = LanguageType;
	LanguageDelegate.Broadcast();
}

const UDataTable* USLTextPoolSubsystem::GetUITextPool()
{
	CheckValidOfUITextPool();
	return UITextPool;
}

const UDataTable* USLTextPoolSubsystem::GetStoryTextPool()
{
	CheckValidOfTextPool(ESLTextDataType::ETD_Story);
	return TextPoolMap[ESLTextDataType::ETD_Story];
}

const UDataTable* USLTextPoolSubsystem::GetTalkTextPool()
{
	CheckValidOfTextPool(ESLTextDataType::ETD_Talk);
	return TextPoolMap[ESLTextDataType::ETD_Talk];
}

const UDataTable* USLTextPoolSubsystem::GetNotifyTextPool()
{
	CheckValidOfTextPool(ESLTextDataType::ETD_Notify);
	return TextPoolMap[ESLTextDataType::ETD_Notify];
}

const UDataTable* USLTextPoolSubsystem::GetObjectiveTextPool()
{
	CheckValidOfTextPool(ESLTextDataType::ETD_Objective);
	return TextPoolMap[ESLTextDataType::ETD_Objective];
}

void USLTextPoolSubsystem::CheckValidOfUITextPool()
{
	if (CurrentPoolLanguageMap.Contains(ESLTextDataType::ETD_UI) &&
		CurrentPoolLanguageMap[ESLTextDataType::ETD_UI] == CurrentLanguage)
	{
		if (IsValid(UITextPool))
		{
			return;
		}
	}

	CheckValidOfTextPoolSettings();

	UITextPool = TextPoolSettings->UITextPoolDataMap[CurrentLanguage].LoadSynchronous();
	CurrentPoolLanguageMap.Add(ESLTextDataType::ETD_UI, CurrentLanguage);
}

void USLTextPoolSubsystem::CheckValidOfTextPool(ESLTextDataType TextDataType)
{
	CheckValidOfLevelSubsystem();

	if (TextPoolChapterMap.Contains(TextDataType) &&
		TextPoolChapterMap[TextDataType] == LevelSubsystem->GetCurrentChapter() &&
		CurrentPoolLanguageMap.Contains(TextDataType) &&
		CurrentPoolLanguageMap[TextDataType] == CurrentLanguage)
	{
		if (TextPoolMap.Contains(TextDataType) &&
			IsValid(TextPoolMap[TextDataType]))
		{
			return;
		}
	}
	
	CheckValidOfTextPoolData(TextDataType);

	TextPoolMap.Add(TextDataType, TextPoolDataMap[TextDataType].LanguageTextTableMap[CurrentLanguage].LoadSynchronous());
	CurrentPoolLanguageMap.Add(TextDataType, CurrentLanguage);

	checkf(IsValid(TextPoolMap[TextDataType]), TEXT("Target Data is invalid"));
}

void USLTextPoolSubsystem::CheckValidOfTextPoolData(ESLTextDataType TextDataType)
{
	CheckValidOfLevelSubsystem();
	ESLChapterType CurrentChapter = LevelSubsystem->GetCurrentChapter();

	if (TextPoolChapterMap.Contains(TextDataType) &&
		TextPoolChapterMap[TextDataType] == CurrentChapter &&
		TextPoolDataMap.Contains(TextDataType))
	{
		return;
	}

	CheckValidOfTextPoolSettings();
	TextPoolChapterMap.Add(TextDataType, CurrentChapter);
	TextPoolDataMap.Add(TextDataType, TextPoolSettings->TextPoolDataMap[CurrentChapter].TextTypeMap[TextDataType]);
}

void USLTextPoolSubsystem::CheckValidOfTextPoolSettings()
{
	if (IsValid(TextPoolSettings))
	{
		return;
	}

	TextPoolSettings = GetDefault<USLTextPoolSettings>();
	checkf(IsValid(TextPoolSettings), TEXT("TextPoolSettings is invalid"));
}

void USLTextPoolSubsystem::CheckValidOfLevelSubsystem()
{
	if (IsValid(LevelSubsystem))
	{
		return;
	}

	LevelSubsystem = GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>();
	checkf(IsValid(LevelSubsystem), TEXT("Level Subsystem is invalid"));
}

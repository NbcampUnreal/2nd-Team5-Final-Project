// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/SLTextPoolSettings.h"

void USLTextPoolSubsystem::OnChangedLanguage(ESLLanguageType LanguageType)
{
	CurrentLanguage = LanguageType;
	LanguageDelegate.Broadcast();
}

const UDataTable* USLTextPoolSubsystem::GetUITextPool()
{
	CheckValidOfTextPool(ESLTextDataType::ETD_UI);
	return TextPoolMap[ESLTextDataType::ETD_UI];
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

const UDataTable* USLTextPoolSubsystem::GetOtherTextPool()
{
	CheckValidOfTextPool(ESLTextDataType::ETD_Other);
	return TextPoolMap[ESLTextDataType::ETD_Other];
}

void USLTextPoolSubsystem::CheckValidOfTextPool(ESLTextDataType TextDataType)
{
	if (CurrentPoolLanguageMap.Contains(TextDataType) && 
		CurrentPoolLanguageMap[TextDataType] == CurrentLanguage)
	{
		if (IsValid(TextPoolMap[TextDataType]))
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
	if (TextPoolDataMap.Contains(TextDataType))
	{
		return;
	}

	CheckValidOfTextPoolSettings();
	TextPoolDataMap.Add(TextDataType, TextPoolSettings->TextPoolDataMap[TextDataType]);
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

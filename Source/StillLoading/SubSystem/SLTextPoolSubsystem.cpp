// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/SLTextPoolSettings.h"


const TMap<ESLLanguageType, UDataTable*>& USLTextPoolSubsystem::GetUITextPool(ESLLanguageType Language)
{
	CheckValidOfTextPool(ESLTextDataType::ETD_UI, Language, UITextPoolMap);
	return UITextPoolMap;
}

const TMap<ESLLanguageType, UDataTable*>& USLTextPoolSubsystem::GetStoryTextPool(ESLLanguageType Language)
{
	CheckValidOfTextPool(ESLTextDataType::ETD_Story, Language, StoryTextPoolMap);
	return StoryTextPoolMap;
}

const TMap<ESLLanguageType, UDataTable*>& USLTextPoolSubsystem::GetTalkTextPool(ESLLanguageType Language)
{
	CheckValidOfTextPool(ESLTextDataType::ETD_Talk, Language, TalkTextPoolMap);
	return TalkTextPoolMap;
}

const TMap<ESLLanguageType, UDataTable*>& USLTextPoolSubsystem::GetNotifyTextPool(ESLLanguageType Language)
{
	CheckValidOfTextPool(ESLTextDataType::ETD_Notify, Language, NotifyTextPoolMap);
	return NotifyTextPoolMap;
}

const TMap<ESLLanguageType, UDataTable*>& USLTextPoolSubsystem::GetOtherTextPool(ESLLanguageType Language)
{
	CheckValidOfTextPool(ESLTextDataType::ETD_Other, Language, OtherTextPoolMap);
	return OtherTextPoolMap;
}

void USLTextPoolSubsystem::CheckValidOfTextPool(ESLTextDataType TextDataType, ESLLanguageType TargetLanguage, TMap<ESLLanguageType, UDataTable*>& TargetMap)
{
	if (TargetMap.Contains(TargetLanguage))
	{
		if (IsValid(TargetMap[TargetLanguage]))
		{
			return;
		}
	}
	
	CheckValidOfTextPoolData(TextDataType);
	
	for (const TPair<ESLTextDataType, FSLTextPoolDataStruct>& TargetData : TextPoolDataMap)
	{
		if (TargetData.Value.LanguageType == TargetLanguage)
		{
			TargetMap.Add(TargetLanguage, TargetData.Value.TextDataTable.LoadSynchronous());
			break;
		}
	}

	checkf(IsValid(TargetMap[TargetLanguage]), TEXT("TextPool Data is invalid"));
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

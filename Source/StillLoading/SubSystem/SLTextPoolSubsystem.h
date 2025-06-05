// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/SLUITypes.h"
#include "SubSystem/SLLevelTransferTypes.h"
#include "SubSystem/SLTextPoolTypes.h"
#include "SubSystem/Struct/SLTextPoolDataStruct.h"
#include "SLTextPoolSubsystem.generated.h"

class USLTextPoolSettings;
class USLLevelTransferSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangedLanguage);

UCLASS()
class STILLLOADING_API USLTextPoolSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void OnChangedLanguage(ESLLanguageType LanguageType);

	const UDataTable* GetUITextPool();
	const UDataTable* GetStoryTextPool();
	const UDataTable* GetTalkTextPool();
	const UDataTable* GetNotifyTextPool();
	const UDataTable* GetObjectiveTextPool();

private:
	void CheckValidOfUITextPool();
	void CheckValidOfTextPool(ESLTextDataType TextDataType);
	void CheckValidOfTextPoolData(ESLTextDataType TextDataType);
	void CheckValidOfTextPoolSettings();
	void CheckValidOfLevelSubsystem();

public:
	UPROPERTY()
	FOnChangedLanguage LanguageDelegate;

private:
	UPROPERTY()
	const USLTextPoolSettings* TextPoolSettings = nullptr;

	UPROPERTY()
	TObjectPtr<USLLevelTransferSubsystem> LevelSubsystem = nullptr;

	UPROPERTY()
	TMap<ESLTextDataType, FSLTextPoolDataStruct> TextPoolDataMap;

	UPROPERTY()
	TMap<ESLTextDataType, ESLLanguageType> CurrentPoolLanguageMap;

	UPROPERTY()
	TMap<ESLTextDataType, UDataTable*> TextPoolMap;

	UPROPERTY()
	TObjectPtr<UDataTable> UITextPool = nullptr;

	ESLChapterType DataChapter = ESLChapterType::EC_None;
	ESLLanguageType CurrentLanguage = ESLLanguageType::EL_Kor;
};

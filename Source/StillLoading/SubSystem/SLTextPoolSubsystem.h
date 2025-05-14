// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/SLUITypes.h"
#include "SubSystem/SLTextPoolTypes.h"
#include "SubSystem/Struct/SLTextPoolDataStruct.h"
#include "SLTextPoolSubsystem.generated.h"

class USLTextPoolSettings;

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
	const UDataTable* GetOtherTextPool();

private:
	void CheckValidOfTextPool(ESLTextDataType TextDataType);
	void CheckValidOfTextPoolData(ESLTextDataType TextDataType);
	void CheckValidOfTextPoolSettings();

public:
	UPROPERTY()
	FOnChangedLanguage LanguageDelegate;

private:
	UPROPERTY()
	const USLTextPoolSettings* TextPoolSettings = nullptr;

	UPROPERTY()
	TMap<ESLTextDataType, FSLTextPoolDataStruct> TextPoolDataMap;

	UPROPERTY()
	TMap<ESLTextDataType, ESLLanguageType> CurrentPoolLanguageMap;

	UPROPERTY()
	TMap<ESLTextDataType, UDataTable*> TextPoolMap;


	ESLLanguageType CurrentLanguage = ESLLanguageType::EL_Kor;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/SLUITypes.h"
#include "SubSystem/SLTextPoolTypes.h"
#include "SubSystem/Struct/SLTextPoolDataStruct.h"
#include "SLTextPoolSubsystem.generated.h"

class USLTextPoolSettings;

UCLASS()
class STILLLOADING_API USLTextPoolSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	const TMap<ESLLanguageType, UDataTable*>& GetUITextPool(ESLLanguageType Language);
	const TMap<ESLLanguageType, UDataTable*>& GetStoryTextPool(ESLLanguageType Language);
	const TMap<ESLLanguageType, UDataTable*>& GetTalkTextPool(ESLLanguageType Language);
	const TMap<ESLLanguageType, UDataTable*>& GetNotifyTextPool(ESLLanguageType Language);
	const TMap<ESLLanguageType, UDataTable*>& GetOtherTextPool(ESLLanguageType Language);

private:

	void CheckValidOfTextPool(ESLTextDataType TextDataType, 
								ESLLanguageType TargetLanguage, 
								TMap<ESLLanguageType, UDataTable*>& TargetMap);

	void CheckValidOfTextPoolData(ESLTextDataType TextDataType);
	void CheckValidOfTextPoolSettings();

private:
	UPROPERTY()
	const USLTextPoolSettings* TextPoolSettings = nullptr;

	UPROPERTY()
	TMap<ESLTextDataType, FSLTextPoolDataStruct> TextPoolDataMap;

	UPROPERTY()
	TMap<ESLLanguageType, UDataTable*> UITextPoolMap;

	UPROPERTY()
	TMap<ESLLanguageType, UDataTable*> StoryTextPoolMap;

	UPROPERTY()
	TMap<ESLLanguageType, UDataTable*> TalkTextPoolMap;

	UPROPERTY()
	TMap<ESLLanguageType, UDataTable*> NotifyTextPoolMap;

	UPROPERTY()
	TMap<ESLLanguageType, UDataTable*> OtherTextPoolMap;
};

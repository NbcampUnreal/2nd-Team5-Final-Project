// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SubSystem/Struct/SLTextPoolDataStruct.h"
#include "SubSystem/SLTextPoolTypes.h"
#include "SubSystem/SLLevelTransferTypes.h"
#include "SLTextPoolSettings.generated.h"


UCLASS(Config = TextPoolSetting, DefaultConfig, meta = (DisplayName = "TextPool Subsystem Settings"))
class STILLLOADING_API USLTextPoolSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Config, Category = "TextPoolData")
	TMap<ESLChapterType, FSLTextTypeStruct> TextPoolDataMap;

	UPROPERTY(EditAnywhere, Config, Category = "TextPoolData")
	TMap<ESLLanguageType, TSoftObjectPtr<UDataTable>> UITextPoolDataMap;
};

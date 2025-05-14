// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubSystem/SLTextPoolTypes.h"
#include "UI/SLUITypes.h"
#include "SLTextPoolDataRows.generated.h"

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLUITextData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TMap<ESLChapterType, FText> ChapterTextMap;
};

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLUITextPoolDataRow : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	ESLTargetWidgetType TargetWidget = ESLTargetWidgetType::ETW_None;

	UPROPERTY(EditAnywhere)
	TMap<FName, FSLUITextData> TextMap;
};

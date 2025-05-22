// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubSystem/SLTextPoolTypes.h"
#include "UI/SLUITypes.h"
#include "SubSystem/SLLevelTransferTypes.h"
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
struct STILLLOADING_API FSLTalkTextData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FName> TalkOwnArray;

	UPROPERTY(EditAnywhere)
	TArray<FText> TalkTextArray;
};

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLNotifyTextData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TMap<ESLNotifyType, FText> NotifyMap;
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

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLStoryTextPoolDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	ESLChapterType Chapter = ESLChapterType::EC_Intro;

	UPROPERTY(EditAnywhere)
	TMap<ESLStoryType, FSLTalkTextData> TextMap;
};

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLTalkTextPoolDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	ESLTalkTargetType TalkTarget = ESLTalkTargetType::ETT_None;

	UPROPERTY(EditAnywhere)
	TMap<int32, FSLTalkTextData> TalkMap;
};

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLNotifyTextPoolDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	ESLChapterType Chapter = ESLChapterType::EC_Intro;

	UPROPERTY(EditAnywhere)
	TMap<ESLGameMapType, FSLNotifyTextData> TextMap;
};

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLOtherTextPoolDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	
};
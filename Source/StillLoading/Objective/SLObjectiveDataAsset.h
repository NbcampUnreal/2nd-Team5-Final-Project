// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SLObjectiveDataAsset.generated.h"

class USLObjectiveBase;
/**
 * 
 */
UCLASS()
class STILLLOADING_API USLObjectiveDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, DisplayName ="챕터 오브젝트 리스트")
	TMap<FName, TObjectPtr<USLObjectiveBase>> ChapterObjectiveMap;

#if WITH_EDITOR
protected:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};

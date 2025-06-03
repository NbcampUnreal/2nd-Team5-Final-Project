// Fill out your copyright notice in the Description page of Project Settings.


#include "SLObjectiveDataAsset.h"

#include "SLObjectiveBase.h"

#if WITH_EDITOR
void USLObjectiveDataAsset::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	for (auto&[Name, Objective] : ChapterObjectiveMap)
	{
		if (Objective == nullptr) continue;
		Objective->SetName(Name);
	}
}
#endif

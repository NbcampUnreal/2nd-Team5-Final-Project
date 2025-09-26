// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SubWidget/SLSaveLoadWidget.h"
#include "Blueprint/WidgetTree.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLSaveSlotWidget.h"
#include "SaveLoad/SLSaveGameSubsystem.h"

void USLSaveLoadWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	Super::InitWidget(NewUISubsystem);

	TArray<UWidget*> FoundWidgets;
	WidgetTree->GetAllWidgets(FoundWidgets);

	int32 Count = 0;

	for (UWidget* Widget : FoundWidgets)
	{
		if (IsValid(Widget))
		{
			USLSaveSlotWidget* SaveSlot = Cast<USLSaveSlotWidget>(Widget);

			if (IsValid(SaveSlot))
			{
				Slots.Add(SaveSlot);
				++Count;
				SaveSlot->InitWidget(Count);
				SaveSlot->OnClicked.AddDynamic(this, &ThisClass::OnClickedSlot);
			}
		}
	}
}

void USLSaveLoadWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::ActivateWidget(WidgetActivateBuffer);

	OnUpdatedSettingValue();
}

void USLSaveLoadWidget::OnUpdatedSettingValue()
{
	Super::OnUpdatedSettingValue();

	for (int32 Number = 0; Number < Slots.Num(); ++ Number)
	{
		UpdateSlotData(Number);
	}
}

void USLSaveLoadWidget::ApplyTextData()
{
	Super::ApplyTextData();
}

void USLSaveLoadWidget::OnClickedSlot(int32 Number)
{
	SelectedSlot = Number;

	// 팝업 출력. 메인 타이틀이면 Load, 그 외는 Save.
}

void USLSaveLoadWidget::OnSaveClicked()
{
	USLSaveGameSubsystem* SaveGameSub = GetGameInstance()->GetSubsystem<USLSaveGameSubsystem>();
	checkf(IsValid(SaveGameSub), TEXT("Save Game Subsystem is invalid"));
}

void USLSaveLoadWidget::OnLoadClicked()
{
	USLSaveGameSubsystem* SaveGameSub = GetGameInstance()->GetSubsystem<USLSaveGameSubsystem>();
	checkf(IsValid(SaveGameSub), TEXT("Save Game Subsystem is invalid"));
}

void USLSaveLoadWidget::OnCancleClicked()
{
	SelectedSlot = 0;

	// 팝업 제거
}

void USLSaveLoadWidget::UpdateSlotData(int32 SlotNum)
{
	USLSaveSlotWidget* SaveSlot = Slots[SlotNum];
	checkf(IsValid(SaveSlot), TEXT("%d Slot is Invalid"), SlotNum);

	USLSaveGameSubsystem* SaveGameSub = GetGameInstance()->GetSubsystem<USLSaveGameSubsystem>();
	checkf(IsValid(SaveGameSub), TEXT("Save Game Subsystem is invalid"));

	TArray<FSlotSaveData> SaveDatas = SaveGameSub->GetSaveSlotList();

	if (SaveDatas.Num() <= SlotNum)
	{
		return;
	}

	FSlotSaveData SlotData = SaveDatas[SlotNum];

	if (SlotData.PlayTimeInSeconds == 0.0f)
	{
		SaveSlot->SetEmptyMode(true);
		return;
	}

	const UEnum* ChapterEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("ESLChapterType"), true);
	FText ChpaterText = FText::GetEmpty();

	if (IsValid(ChapterEnum))
	{
		ChpaterText = ChapterEnum->GetDisplayNameTextByValue(static_cast<int64>(SlotData.ChapterSaveData));
	}

	SaveSlot->SetChapterNumber(ChpaterText);

	const UEnum* LevelEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("ESLLevelNameType"), true);
	FText LevelText = FText::GetEmpty();

	if (IsValid(LevelEnum))
	{
		LevelText = LevelEnum->GetDisplayNameTextByValue(static_cast<int64>(SlotData.LevelSaveData));
	}

	SaveSlot->SetMapName(LevelText);
	//Slot->SetMapImage();

	SaveSlot->SetPlayTime(SlotData.PlayTimeInSeconds);
	SaveSlot->SetSaveTime(SlotData.SaveTime);

	SaveSlot->SetEmptyMode(false);
}

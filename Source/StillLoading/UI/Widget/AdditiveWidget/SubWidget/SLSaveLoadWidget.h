// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLOptionSubBase.h"
#include "SLSaveLoadWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
class UCanvasPanel;
class USLSaveSlotWidget;
class USLSaveSlotPrivateDataAsset;

UCLASS()
class STILLLOADING_API USLSaveLoadWidget : public USLOptionSubBase
{
	GENERATED_BODY()

public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;
	virtual void OnUpdatedSettingValue() override;

protected:
	virtual void ApplyFontData() override;
	virtual void ApplyTextData() override;
	virtual bool ApplySlotImage(FSlateBrush& SlateBrush) override;
	virtual bool ApplySavePopImage(FSlateBrush& SlateBrush) override;

private:
	UFUNCTION()
	void OnClickedSlot(int32 Number);

	UFUNCTION()
	void OnSaveClicked();

	UFUNCTION()
	void OnLoadClicked();

	UFUNCTION()
	void OnCancleClicked();

	void UpdateSlotData(int32 SlotNum);

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USLSaveSlotPrivateDataAsset> SlotDataAsset = nullptr;

private:
	UPROPERTY()
	TArray<TObjectPtr<USLSaveSlotWidget>> Slots;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> AgreeButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> CancleButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> PopBoxImg = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> NotiText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> NotiText2 = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCanvasPanel> NotiBox = nullptr;

	int32 SelectedSlot = 0;
	bool bIsMainTitle = true;

	static const FName SaveTextIndex;
	static const FName SaveTextIndex2;
};

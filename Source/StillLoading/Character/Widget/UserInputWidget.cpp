#include "UserInputWidget.h"

#include "Character/DynamicIMCComponent/DynamicIMCComponent.h"

void UUserInputWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (TempButton)
	{
		TempButton->OnClicked.AddDynamic(this, &UUserInputWidget::OnTempButtonClicked);
	}
}

void UUserInputWidget::OnTempButtonClicked()
{
	StartRebind();
}

void UUserInputWidget::StartRebind()
{
	bWaitingForInput = true;
	
	SetIsFocusable(true);
	SetKeyboardFocus();
	
	UE_LOG(LogTemp, Display, TEXT("StartRebind"));
}

FReply UUserInputWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!bWaitingForInput)
	{
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	}
	
	const FKey PressedKey = InKeyEvent.GetKey();
	UE_LOG(LogTemp, Warning, TEXT("Pressed Key: %s"), *PressedKey.ToString());

	bWaitingForInput = false;
	FSlateApplication::Get().ClearKeyboardFocus(EFocusCause::SetDirectly);

	if (LinkedCharacter)
	{
		auto* IMCComp = LinkedCharacter->FindComponentByClass<UDynamicIMCComponent>();
		if (IMCComp)
		{
			IMCComp->SetKeyForAction(EInputActionType::Jump, PressedKey);
		}
	}

	return FReply::Handled();
}
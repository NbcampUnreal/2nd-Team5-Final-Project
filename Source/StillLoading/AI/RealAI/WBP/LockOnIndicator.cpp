#include "LockOnIndicator.h"

#include "Components/Image.h"

void ULockOnIndicator::NativeConstruct()
{
	Super::NativeConstruct();

	if (OutlineImage)
	{
		OutlineImage->SetColorAndOpacity(FLinearColor::Green);
	}
}

void ULockOnIndicator::SetOutlineColor(const FLinearColor Color)
{
	if (OutlineImage)
	{
		OutlineImage->SetColorAndOpacity(Color);
	}
}
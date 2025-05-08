#include "SLBasePlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Character/SLBaseCharacter.h"

void ASLBasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (InputWidgetClass)
	{
		InputWidget = CreateWidget<UUserInputWidget>(this, InputWidgetClass);

		if (InputWidget)
		{
			InputWidget->AddToViewport();

			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(InputWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);

			InputWidget->LinkedCharacter = Cast<ASLBaseCharacter>(GetPawn());

			bShowMouseCursor = true;
		}
	}
}

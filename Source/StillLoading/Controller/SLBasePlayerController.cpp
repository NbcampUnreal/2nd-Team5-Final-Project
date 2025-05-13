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

			InputWidget->LinkedCharacter = Cast<ASLBaseCharacter>(GetPawn());

			bShowMouseCursor = false;
		}
	}

	//팀 번호 설정
	playerTeamId = FGenericTeamId(0);
}

FGenericTeamId ASLBasePlayerController::GetGenericTeamId() const
{
	return playerTeamId;
}

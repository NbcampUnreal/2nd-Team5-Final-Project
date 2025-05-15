#include "SLBasePlayerController.h"

void ASLBasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	//팀 번호 설정
	playerTeamId = FGenericTeamId(0);
}

FGenericTeamId ASLBasePlayerController::GetGenericTeamId() const
{
	return playerTeamId;
}

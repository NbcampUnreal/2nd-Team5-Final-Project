#include "SLBasePlayerController.h"

#include "Character/SLPlayerCharacter.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"

void ASLBasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	//팀 번호 설정
	playerTeamId = FGenericTeamId(0);
}

void ASLBasePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(LogTemp, Warning, TEXT("Possessed Pawn[%s]"), *InPawn->GetName());

	if (const ASLPlayerCharacter* PlayerCharacter = Cast<ASLPlayerCharacter>(InPawn))
	{
		UDynamicIMCComponent* DynamicIMCComponent = PlayerCharacter->FindComponentByClass<UDynamicIMCComponent>();
		DynamicIMCComponent->BindDefaultSetting();
	}
}

FGenericTeamId ASLBasePlayerController::GetGenericTeamId() const
{
	return playerTeamId;
}

void ASLBasePlayerController::BindIMC(APawn* InPawn)
{
	if (const ASLPlayerCharacter* PlayerCharacter = Cast<ASLPlayerCharacter>(InPawn))
	{
		UDynamicIMCComponent* DynamicIMCComponent = PlayerCharacter->FindComponentByClass<UDynamicIMCComponent>();
		DynamicIMCComponent->BindDefaultSetting();
	}
}

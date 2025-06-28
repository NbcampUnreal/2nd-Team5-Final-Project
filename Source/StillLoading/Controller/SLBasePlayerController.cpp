#include "SLBasePlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/BattleComponent/SLTargetableInterface.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "UI/HUD/SLInGameHUD.h"

ASLBasePlayerController::ASLBasePlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASLBasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	//팀 번호 설정
	PlayerTeamId = FGenericTeamId(0);

	if (DefaultCursorWidgetClass)
	{
		DefaultCursorWidgetInstance = CreateWidget<UUserWidget>(this, DefaultCursorWidgetClass);
		SetDefaultCursor();
	}
	if (EnemyCursorWidgetClass)
	{
		EnemyCursorWidgetInstance = CreateWidget<UUserWidget>(this, EnemyCursorWidgetClass);
	}
}

void ASLBasePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckActorUnderCursor();
}

void ASLBasePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	BindIMC(InPawn);

	GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::ShowInGameUI);
}

void ASLBasePlayerController::SetIgnoreMoveInput(bool bNewMoveInput)
{
	Super::SetIgnoreMoveInput(bNewMoveInput);

	if (bNewMoveInput)
	{
		HideInGameUI();
	}
	else
	{
		ShowInGameUI();
	}
}

FGenericTeamId ASLBasePlayerController::GetGenericTeamId() const
{
	return PlayerTeamId;
}

void ASLBasePlayerController::SetDefaultCursor()
{
	SetMouseCursorWidget(EMouseCursor::Default, DefaultCursorWidgetInstance);
}

void ASLBasePlayerController::SetEnemyCursor()
{
	SetMouseCursorWidget(EMouseCursor::Default, EnemyCursorWidgetInstance);
}

void ASLBasePlayerController::BindIMC(APawn* InPawn)
{
	if (const ASLPlayerCharacter* PlayerCharacter = Cast<ASLPlayerCharacter>(InPawn))
	{
		UDynamicIMCComponent* DynamicImcComponent = PlayerCharacter->FindComponentByClass<UDynamicIMCComponent>();
		DynamicImcComponent->BindDefaultSetting();
	}
}

void ASLBasePlayerController::ShowInGameUI()
{
	ASLInGameHUD* HUD = Cast<ASLInGameHUD>(GetHUD());

	if (IsValid(HUD))
	{
		HUD->ApplyObjective();
		HUD->SetVisibilityPlayerState(true);
	}
}

void ASLBasePlayerController::HideInGameUI()
{
	ASLInGameHUD* HUD = Cast<ASLInGameHUD>(GetHUD());

	if (IsValid(HUD))
	{
		HUD->SetInvisibleObjectivve();
		HUD->SetVisibilityPlayerState(false);
	}
}

void ASLBasePlayerController::CheckActorUnderCursor()
{
	FVector2D CurrentMousePosition;
	if (!GetMousePosition(CurrentMousePosition.X, CurrentMousePosition.Y))
	{
		return;
	}

	if (CurrentMousePosition.Equals(LastMousePosition, 0.1f))
	{
		return;
	}
	
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_GameTraceChannel1, false, HitResult);

	if (HitResult.bBlockingHit)
	{
		//DrawDebugSphere(GetWorld(), HitResult.Location, 25.f, 12, FColor::Red, false, 1.0f);
	}

	AActor* CurrentHoveredActor = HitResult.GetActor();
	if (CurrentHoveredActor != HoveredActor_LastFrame)
	{
		if (HoveredActor_LastFrame)
		{
			if (HoveredActor_LastFrame->GetClass()->ImplementsInterface(USLTargetableInterface::StaticClass()))
			{
				ISLTargetableInterface::Execute_OnUnhoveredByCursor(HoveredActor_LastFrame, this);
			}
		}
		if (CurrentHoveredActor)
		{
			if (CurrentHoveredActor->GetClass()->ImplementsInterface(USLTargetableInterface::StaticClass()))
			{
				ISLTargetableInterface::Execute_OnHoveredByCursor(CurrentHoveredActor, this);
			}
		}
	}
	
	HoveredActor_LastFrame = CurrentHoveredActor;
	LastMousePosition = CurrentMousePosition;
}

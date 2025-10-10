#include "SLBasePlayerController.h"

#include "AI/RealAI/BattleManager/SLBattleManager.h"
#include "Blueprint/UserWidget.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/BattleComponent/SLTargetableInterface.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/SLInGameHUD.h"
#include "SubSystem/SLDemoSubsystem.h"

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

	ASLBattleManager* BattleManager = nullptr;
	if (!BattleManager)
	{
		BattleManager = Cast<ASLBattleManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASLBattleManager::StaticClass()));
	}

	if (BattleManager)
	{
		BattleManager->RegisterPlayerUnit(this);
		UE_LOG(LogTemp, Log, TEXT("AI 유닛 '%s': BattleManager에 성공적으로 등록됨."), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AI 유닛 '%s': BattleManager를 찾을 수 없어 등록에 실패했습니다."), *GetName());
	}
}

void ASLBasePlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	HideInGameUI();
}

void ASLBasePlayerController::SetIgnoreMoveInput(bool bNewMoveInput)
{
	Super::SetIgnoreMoveInput(bNewMoveInput);

	USLDemoSubsystem* DemoSub = GetGameInstance()->GetSubsystem<USLDemoSubsystem>();
	checkf(IsValid(DemoSub), TEXT("DemoSub is invalid"));

	if (bNewMoveInput)
	{
		DemoSub->PauseTimer(true);
		HideInGameUI();
	}
	else
	{
		ShowInGameUI();
		DemoSub->ContinueTimer();
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
		HUD->ShowInGameWidget(true);
	}
}

void ASLBasePlayerController::HideInGameUI()
{
	ASLInGameHUD* HUD = Cast<ASLInGameHUD>(GetHUD());

	if (IsValid(HUD))
	{
		HUD->ShowInGameWidget(false);
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

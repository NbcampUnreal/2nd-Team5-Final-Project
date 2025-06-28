#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "SLBasePlayerController.generated.h"

UCLASS()
class STILLLOADING_API ASLBasePlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	ASLBasePlayerController();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetIgnoreMoveInput(bool bNewMoveInput) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

	UFUNCTION(BlueprintCallable)
	void SetDefaultCursor();
	UFUNCTION(BlueprintCallable)
	void SetEnemyCursor();

	UFUNCTION(BlueprintCallable)
	void BindIMC(APawn* InPawn);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Cursor")
	TSubclassOf<UUserWidget> DefaultCursorWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Cursor")
	TSubclassOf<UUserWidget> EnemyCursorWidgetClass;
	
private:
	UFUNCTION()
	void ShowInGameUI();

	UFUNCTION()
	void HideInGameUI();

	void CheckActorUnderCursor();

	UPROPERTY()
	FGenericTeamId PlayerTeamId;

	UPROPERTY()
	TObjectPtr<UUserWidget> DefaultCursorWidgetInstance;

	UPROPERTY()
	TObjectPtr<UUserWidget> EnemyCursorWidgetInstance;

	UPROPERTY()
	TObjectPtr<AActor> HoveredActor_LastFrame;

	UPROPERTY()
	FVector2D LastMousePosition;
};

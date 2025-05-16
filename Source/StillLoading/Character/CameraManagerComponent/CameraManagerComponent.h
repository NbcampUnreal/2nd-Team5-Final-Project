#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/CameraManagerComponent/SLCameraManagerActor.h"
#include "Character/CameraManagerComponent/SLCameraType.h"
#include "CameraManagerComponent.generated.h"

class USpringArmComponent;
class UCameraComponent;



UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UCameraManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCameraManagerComponent();

	UFUNCTION()
	void SwitchCamera(const EGameCameraType NewType);
	UFUNCTION()
	void SetCameraRefsofCharacter(UCameraComponent* InThirdPerson, UCameraComponent* InFirstPerson,UCameraComponent* InSideView);
	
	EGameCameraType GetCurrnetCameraMode();

protected:
	virtual void BeginPlay() override;

private:

	void SpawnCameraActor();

	void HideCamera(UCameraComponent* Camera);

	UPROPERTY()
	TObjectPtr<UCameraComponent> ThirdPersonCamera;

	UPROPERTY()
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY()
	TObjectPtr<UCameraComponent> SideViewCamera;

	UPROPERTY()
	TObjectPtr<ASLCameraManagerActor> CameraManagerActor;



	UPROPERTY()
	EGameCameraType CurrentCameraMode = EGameCameraType::EGCT_ThirdPerson;

};

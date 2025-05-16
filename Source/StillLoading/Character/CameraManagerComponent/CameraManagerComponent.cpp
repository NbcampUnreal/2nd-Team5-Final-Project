#include "CameraManagerComponent.h"

#include "Character/SLBaseCharacter.h"
#include "Camera/CameraComponent.h"

void UCameraManagerComponent::SetCameraRefsofCharacter(
    UCameraComponent* InThirdPerson, UCameraComponent* InFirstPerson, UCameraComponent* InSideView)
{
    FirstPersonCamera = InThirdPerson;
    ThirdPersonCamera = InFirstPerson;
	SideViewCamera = InSideView;
}

EGameCameraType UCameraManagerComponent::GetCurrnetCameraMode()
{
    return CurrentCameraMode;
}

void UCameraManagerComponent::BeginPlay()
{
	Super::BeginPlay();
    SpawnCameraActor();
    SwitchCamera(CurrentCameraMode);

}

void UCameraManagerComponent::SpawnCameraActor()
{
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

   
    FVector SpawnLocation = GetOwner()->GetActorLocation();
    FRotator SpawnRotation = FRotator::ZeroRotator;

    CameraManagerActor = GetWorld()->SpawnActor<ASLCameraManagerActor>(
        ASLCameraManagerActor::StaticClass(),
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );
}

void UCameraManagerComponent::HideCamera(UCameraComponent* Camera)
{
    if (Camera)
    {
        Camera->SetActive(false);
    }
}

UCameraManagerComponent::UCameraManagerComponent()
{
    CameraManagerActor = nullptr;
}

void UCameraManagerComponent::SwitchCamera(const EGameCameraType NewMode)
{
    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (!OwnerChar) return;

    APlayerController* PC = Cast<APlayerController>(OwnerChar->GetController());
    if (!PC) return;

    CurrentCameraMode = NewMode;

    switch (NewMode)
    {
    case EGameCameraType::EGCT_ThirdPerson: //3ÀÎÄª
        if (ThirdPersonCamera)
        {
            ThirdPersonCamera->SetActive(true);
            HideCamera(FirstPersonCamera);
            HideCamera(SideViewCamera);
            PC->SetViewTargetWithBlend(OwnerChar, 0.3f);
        }
        break;

    case EGameCameraType::EGCT_FirstPerson: //1ÀÎÄª
        if (FirstPersonCamera)
        {
            FirstPersonCamera->SetActive(true);
            HideCamera(ThirdPersonCamera);
            HideCamera(SideViewCamera);

            PC->SetViewTargetWithBlend(OwnerChar, 0.3f);
        }
        break;

    case EGameCameraType::EGCT_SideView: //¿·
        if (SideViewCamera)
        {
            SideViewCamera->SetActive(true);
           
            HideCamera(FirstPersonCamera);
            HideCamera(ThirdPersonCamera);

            PC->SetViewTargetWithBlend(OwnerChar, 0.3f);
        }
        break;

    case EGameCameraType::EGCT_TopDown: //Å¾´Ù¿î, ½Ã³×¸¶Æ½ ¿ÜºÎ Ä«¸Þ¶ó ¾×ÅÍ
    case EGameCameraType::EGCT_Cinematic:
        if (CameraManagerActor)
        {
            HideCamera(FirstPersonCamera);
            HideCamera(ThirdPersonCamera);
            HideCamera(SideViewCamera);
            CameraManagerActor->SetCameraMode(NewMode);
        }
        break;
    }
}



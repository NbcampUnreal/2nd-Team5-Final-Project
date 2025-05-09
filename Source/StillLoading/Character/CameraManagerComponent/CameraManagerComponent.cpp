#include "CameraManagerComponent.h"

#include "Camera/CameraComponent.h"

void UCameraManagerComponent::SetCameraRefs(UCameraComponent* InDefault, UCameraComponent* InBattle,
	UCameraComponent* InTopDown, UCameraComponent* InSideView)
{
	DefaultCamera = InDefault;
	BattleCamera = InBattle;
	TopDownCamera = InTopDown;
	SideViewCamera = InSideView;
}

void UCameraManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentCamera = DefaultCamera;
	if (CurrentCamera)
	{
		CurrentCamera->Activate();
		UpdateView();
	}
	CurrentType = EGameCameraType::EGCT_Default;
}

void UCameraManagerComponent::SwitchCamera(const EGameCameraType NewType)
{
	UCameraComponent* NewCam = GetCameraByType(NewType);
	if (!NewCam || NewCam == CurrentCamera)
		return;

	if (CurrentCamera) CurrentCamera->Deactivate();
	NewCam->Activate();
	CurrentCamera = NewCam;
	CurrentType = NewType;

	UpdateView();
}

void UCameraManagerComponent::UpdateView()
{
	APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController());
	if (PC && CurrentCamera)
	{
		PC->SetViewTargetWithBlend(GetOwner(), 0.4f);
	}
}

UCameraComponent* UCameraManagerComponent::GetCameraByType(const EGameCameraType Type) const
{
	switch (Type)
	{
	case EGameCameraType::EGCT_Default: return DefaultCamera;
	case EGameCameraType::EGCT_Battle: return BattleCamera;
	case EGameCameraType::EGCT_TopDown: return TopDownCamera;
	case EGameCameraType::EGCT_SideView: return SideViewCamera;
	default: return nullptr;
	}
}
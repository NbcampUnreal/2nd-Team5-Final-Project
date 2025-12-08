#include "SLInteractableHighlight.h"

#include "Components/PointLightComponent.h"
#include "SubSystem/SLLevelTransferSubsystem.h"

USLInteractableHighlight::USLInteractableHighlight()
{
	PrimaryComponentTick.bCanEverTick = true;

	PointLight = CreateDefaultSubobject<UPointLightComponent>("PointLight");
	PointLight->SetupAttachment(this);
}

void USLInteractableHighlight::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bIsInteracted) return;
	
	CurrentTime += DeltaTime * LightSpeed;
	PointLight->Intensity = (sin(CurrentTime) + 1) * LightIntensityMax;
}

void USLInteractableHighlight::BeginPlay()
{
	Super::BeginPlay();
	HighlightDeactivate();
	SetupLight();
}

void USLInteractableHighlight::SetupLight()
{
	PointLight->CastShadows = false;
	PointLight->AttenuationRadius = 300.0f;
	
	if(const USLLevelTransferSubsystem* LevelTransferSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>())
	{
		switch(ESLChapterType ChapterType = LevelTransferSubsystem->GetCurrentChapter())
		{
		default:
			break;
		case ESLChapterType::EC_Chapter0:
		case ESLChapterType::EC_Chapter1:
		case ESLChapterType::EC_Chapter2:
		case ESLChapterType::EC_Chapter3:
			PointLight->InverseExposureBlend = 1;
			break;
		}
	}
}

void USLInteractableHighlight::HighlightActivate()
{
	SetComponentTickEnabled(true);
}

void USLInteractableHighlight::HighlightDeactivate()
{
	SetComponentTickEnabled(false);
	PointLight->Intensity = 0;
}

void USLInteractableHighlight::SetInteracted(bool bInput)
{
	bIsInteracted = bInput;
}


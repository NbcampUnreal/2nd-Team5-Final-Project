#include "SLFootstepNotify.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "SubSystem/SLSoundSubsystem.h"

USLFootstepNotify::USLFootstepNotify()
{
	TraceDistance = 100.0f;
	TraceChannel = ECC_Visibility;
}

void USLFootstepNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                               const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!IsValid(MeshComp))
	{
		return;
	}

	const UWorld* World = MeshComp->GetWorld();
	if (!World || !World->IsGameWorld())
	{
		return;
	}

	FHitResult HitResult;
	if (PerformGroundTrace(MeshComp, HitResult))
	{
		PlayFootstepSound(HitResult, HitResult.Location);
	}
}

bool USLFootstepNotify::PerformGroundTrace(USkeletalMeshComponent* MeshComp, FHitResult& OutHitResult)
{
	const AActor* Owner = MeshComp->GetOwner();
	if (!IsValid(Owner))
	{
		return false;
	}

	const FVector StartLocation = Owner->GetActorLocation();
	const FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, TraceDistance);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(const_cast<AActor*>(Owner));

	return UKismetSystemLibrary::LineTraceSingle(
		MeshComp->GetWorld(),
		StartLocation,
		EndLocation,
		UEngineTypes::ConvertToTraceType(TraceChannel),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHitResult,
		true
	);
}

void USLFootstepNotify::PlayFootstepSound(const FHitResult& HitResult, const FVector& Location)
{
	const UWorld* World = HitResult.GetActor()->GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(World);
	if (!IsValid(GameInstance))
	{
		return;
	}

	USLSoundSubsystem* SoundSubsystem = GameInstance->GetSubsystem<USLSoundSubsystem>();
	if (!IsValid(SoundSubsystem))
	{
		return;
	}

	// 물리 서피스 타입에 따른 사운드 타입 결정
	EBattleSoundType SoundType = EBattleSoundType::FootstepDefault;
	
	const EPhysicalSurface SurfaceType = UGameplayStatics::GetSurfaceType(HitResult);
	switch (SurfaceType)
	{
	case SurfaceType1: // rock
		SoundType = EBattleSoundType::FootstepRock;
		break;
	case SurfaceType2: // soil
		SoundType = EBattleSoundType::FootstepSoil;
		break;
	default:
		SoundType = EBattleSoundType::FootstepDefault;
		break;
	}

	SoundSubsystem->PlayBattleSound(SoundType, Location);
}
#include "SLSoundEmitterObject.h"

#include "Character/SLPlayerCharacterBase.h"
#include "Perception/AISense_Hearing.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ASLSoundEmitterObject::ASLSoundEmitterObject()
{
	DefaultSoundLevel = ESoundType::EST_Normal;
	bEmitSoundOnInteraction = true;
	bEmitSoundOnHit = true;
}

void ASLSoundEmitterObject::EmitSound(ESoundType SoundLevel, APawn* SoundInstigator)
{
	float Loudness = 1.0f;
	float Range = 800.0f;

	switch (SoundLevel)
	{
	case ESoundType::EST_Quiet:
		Loudness = 1.5f;   // 1.0f → 1.5f
		Range = 1500.0f;   // 1000.0f → 1500.0f
		break;
	case ESoundType::EST_Normal:
		Loudness = 3.0f;   // 2.0f → 3.0f
		Range = 2500.0f;   // 1500.0f → 2500.0f
		break;
	case ESoundType::EST_Loud:
		Loudness = 5.0f;   // 3.0f → 5.0f
		Range = 3500.0f;   // 2000.0f → 3500.0f
		break;
	}

	EmitSoundWithParams(Loudness, Range, SoundInstigator);
}

void ASLSoundEmitterObject::EmitSoundWithParams(float Loudness, float Range, APawn* SoundInstigator)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Instigator 설정 - SoundInstigator가 없으면 플레이어를 찾아서 설정
	APawn* ActualInstigator = SoundInstigator;
	if (!ActualInstigator)
	{
		ActualInstigator = UGameplayStatics::GetPlayerPawn(World, 0);
	}

	// AI에게 소리 이벤트 보고
	UAISense_Hearing::ReportNoiseEvent(
		World,
		GetActorLocation(),
		Loudness,
		ActualInstigator,
		Range,
		FName("ObjectInteraction")
	);

	// 실제 사운드 재생
	if (InteractionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this, 
			InteractionSound, 
			GetActorLocation(),
			FMath::Clamp(Loudness, 0.1f, 2.0f)
		);
	}
}

void ASLSoundEmitterObject::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	Super::OnInteracted(InCharacter, InTriggerType);

	// 캐릭터를 APawn으로 캐스팅
	APawn* CharacterPawn = const_cast<ASLPlayerCharacterBase*>(InCharacter);

	if (InTriggerType == ESLReactiveTriggerType::ERT_Hit && bEmitSoundOnHit)
	{
		EmitSound(DefaultSoundLevel, CharacterPawn);
	}
	else if (InTriggerType == ESLReactiveTriggerType::ERT_InteractKey && bEmitSoundOnInteraction)
	{
		EmitSound(ESoundType::EST_Quiet, CharacterPawn);
	}
}
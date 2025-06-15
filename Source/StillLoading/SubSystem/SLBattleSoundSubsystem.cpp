#include "SLBattleSoundSubsystem.h"

#include "SLBattleSoundSettings.h"
#include "Kismet/GameplayStatics.h"

void USLBattleSoundSubsystem::PlaySoundAtLocation(const EBattleSoundType SoundType, const FVector& Location) const
{
	const USLBattleSoundSettings* BattleSoundSettings = GetDefault<USLBattleSoundSettings>();
	if (!BattleSoundSettings)
	{
		UE_LOG(LogTemp, Warning, TEXT("BattleSoundSettings를 찾을 수 없습니다."));
		return;
	}

	if (const TSoftObjectPtr<USoundBase>* FoundSoundPtr = BattleSoundSettings->BattleSoundMap.Find(SoundType))
	{
		if (USoundBase* SoundToPlay = FoundSoundPtr->LoadSynchronous())
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundToPlay, Location);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load sound for type: %s"), *UEnum::GetValueAsString(SoundType));
		}
	}
}

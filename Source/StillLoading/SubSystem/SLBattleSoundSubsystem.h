#pragma once

#include "CoreMinimal.h"
#include "SubSystem/SLSoundSubsystem.h"
#include "SLBattleSoundSubsystem.generated.h"

enum class EBattleSoundType : uint8;

UCLASS()
class STILLLOADING_API USLBattleSoundSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * 지정된 ID의 공격 사운드를 특정 위치에서 재생합니다.
	 * @param SoundType - Battle Sound Settings에 정의된 사운드의 Key (EBattleSoundType)
	 * @param Location - 사운드가 재생될 위치
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Sound")
	void PlaySoundAtLocation(const EBattleSoundType SoundType, const FVector& Location) const;
};

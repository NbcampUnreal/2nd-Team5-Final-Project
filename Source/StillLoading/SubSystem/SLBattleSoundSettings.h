#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SLBattleSoundSettings.generated.h"

UENUM(BlueprintType)
enum class EBattleSoundType : uint8
{
	BST_Attack			UMETA(DisplayName = "Attack"),
	BST_Hit				UMETA(DisplayName = "Hit"),
	BST_Walk			UMETA(DisplayName = "Walk"),
	BST_Spirit			UMETA(DisplayName = "Spirit"),
	BST_Guard			UMETA(DisplayName = "Guard"),
	BST_Skill			UMETA(DisplayName = "Skill"),
};

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Battle Sound Settings"))
class STILLLOADING_API USLBattleSoundSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// 공격 사운드 목록
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Attack Sounds")
	TMap<EBattleSoundType, TSoftObjectPtr<USoundBase>> BattleSoundMap;
};

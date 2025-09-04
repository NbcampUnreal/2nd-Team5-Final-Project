#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "SLBattleTypes.generated.h"

struct FWaveCompositionData;
// 전방 선언
class ASLSwarmSpawner;
class ATargetPoint;
class AActor;

UENUM(BlueprintType)
enum class EAILODLevel : uint8
{
	Max		UMETA(DisplayName = "Max Detail"),
	High	UMETA(DisplayName = "High Detail"),
	Medium	UMETA(DisplayName = "Medium Detail"),
	Low		UMETA(DisplayName = "Low Detail"),
	Culled	UMETA(DisplayName = "Culled")
};


USTRUCT(BlueprintType)
struct FBattleUnitInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AActor> Actor = nullptr;
	UPROPERTY()
	FGenericTeamId TeamId;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> CurrentEngagedTarget;
	UPROPERTY()
	TObjectPtr<ASLSwarmSpawner> SourceSpawner = nullptr;
};

USTRUCT(BlueprintType)
struct FWaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TArray<FWaveCompositionData> WaveCompositions;
};

USTRUCT(BlueprintType)
struct FTeamIndicesArrayWrapper
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<int32> Indices;
};

USTRUCT(BlueprintType)
struct FEngagedUnitsWrapper
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<AActor>> EngagedUnits;
};

USTRUCT(BlueprintType)
struct FBattleAILODBudget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI LOD Budget")
	int32 MaxLODCount = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI LOD Budget")
	int32 HighLODCount = 25;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI LOD Budget")
	int32 MediumLODCount = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI LOD Budget")
	int32 LowLODCount = 100;
};

USTRUCT(BlueprintType)
struct FLODDistanceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI LOD Settings")
	float MaxDetailDistance = 1500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI LOD Settings")
	float HighDetailDistance = 2000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI LOD Settings")
	float MediumDetailDistance = 3000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI LOD Settings")
	float LowDetailDistance = 4000.f;
};
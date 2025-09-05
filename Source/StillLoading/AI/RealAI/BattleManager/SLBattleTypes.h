#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "SLBattleTypes.generated.h"

class ASLSwarmSpawner;
class ATargetPoint;
class AActor;
class ACharacter;
class AAIController;

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
struct FWaveCompositionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TSubclassOf<ACharacter> UnitClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TSubclassOf<AAIController> ControllerClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 SpawnCount = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	FGenericTeamId TeamID = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float AvoidanceWeight = 1.0f;
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
	int32 MaxLODCount = 25;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI LOD Budget")
	int32 HighLODCount = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI LOD Budget")
	int32 MediumLODCount = 35;
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
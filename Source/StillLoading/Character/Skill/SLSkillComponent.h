#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AI/RealAI/Spawner/MonsterSpawner.h"
#include "SLSkillComponent.generated.h"

class AFallingSword;
class AMonsterSpawner;

UENUM(BlueprintType)
enum class EActiveSkillType : uint8
{
	AST_Spawn			UMETA(DisplayName = "Spawn"),
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USLSkillComponent();

	UFUNCTION()
	void ActivateSkill(EActiveSkillType SkillType);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn Actor")
	FMonsterSpawnInfo SpawnInfo;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill|Spawn Actor")
	TSubclassOf<AFallingSword> SwordClassToSpawn;

	UPROPERTY()
	TObjectPtr<AMonsterSpawner> SpawnerInstance;

private:
	UFUNCTION()
	void ActiveSpawnSkill();

	UFUNCTION()
	void ActiveSpawnSkillTemp();
};

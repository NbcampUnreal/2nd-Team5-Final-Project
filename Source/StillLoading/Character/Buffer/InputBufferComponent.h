#pragma once

#include "CoreMinimal.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Components/ActorComponent.h"
#include "InputBufferComponent.generated.h"

class ASLPlayerCharacter;
class UMovementHandlerComponent;

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	ST_Attack UMETA(DisplayName = "Attack"),
	ST_Block UMETA(DisplayName = "Block"),
	ST_PointMove UMETA(DisplayName = "PointMove"),
};

USTRUCT(BlueprintType)
struct FBufferedInput
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESkillType Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Timestamp;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UInputBufferComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInputBufferComponent();

	UFUNCTION()
	void AddBufferedInput(ESkillType Action);
	UFUNCTION()
	void ProcessBufferedInputs();
	UFUNCTION()
	void ClearBuffer();
	UFUNCTION()
	void OnIMCActionStarted(EInputActionType ActionType);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Input Buffer")
	float BufferDuration = 1.2f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Input Buffer")
	int32 MaxInputBufferCount = 5;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	UFUNCTION()
	void ExecuteInput(ESkillType Action);
	UFUNCTION()
	bool CanConsumeInput(ESkillType NextInput) const;

	UPROPERTY()
	TObjectPtr<ASLPlayerCharacter> OwnerCharacter;
	UPROPERTY()
	TObjectPtr<UMovementHandlerComponent> CachedMovementHandlerComponent;
	UPROPERTY()
	TArray<FBufferedInput> InputBuffer;
	UPROPERTY()
	float LastInputTime = -1.0f;
};

#pragma once

#include "Character/RunnerTypes.h"
#include "CoreMinimal.h"
#include "LevelSequenceActor.h"
#include "GameFramework/Actor.h"
#include "SLRunnerHurdle.generated.h"

class ALevelSequenceActor;
class UBoxComponent;
class USceneComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQTEKeyChosen, const FString&, Key);

UCLASS()
class STILLLOADING_API ASLRunnerHurdle : public AActor
{
	GENERATED_BODY()
	
public:
	ASLRunnerHurdle();
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintPure)
	EHurdleState GetHurdleState() const { return HurdleState; }
	UFUNCTION(BlueprintPure)
	ERunnerMontageSection GetRunnerMontageSection() const { return RunnerMontageSection; }
	UFUNCTION(BlueprintPure)
	ERunnerMontageSection GetFailMontageSection() const { return FailMontageSection; }
	UFUNCTION(BlueprintCallable)
	void SetDefaultHurdleState() { HurdleState = EHurdleState::None; }
	UFUNCTION(BlueprintPure)
	FString GetExpectedInputKey() const { return ExpectedInputKey; }
	ALevelSequenceActor* GetSuccessLevelSequence() const {return SuccessLevelSequence; }
	ALevelSequenceActor* GetFailLevelSequence() const {return FailLevelSequence; }
	void SetUsed(bool bUsed) { bIsUsed = bUsed; }
	// BP에서 키 표시용 브로드캐스트
	UPROPERTY(BlueprintAssignable, Category="Runner|QTE")
	FOnQTEKeyChosen OnQTEKeyChosen;

protected:
	UFUNCTION()
	void OnWarningOverlap(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIdx, bool bFromSweep, const FHitResult& Hit);
	UFUNCTION()
	void OnMainOverlap(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIdx, bool bFromSweep, const FHitResult& Hit);
	UFUNCTION(BlueprintImplementableEvent)
	void CreateWidget();
	void RandomizeExpectedInputKey();
	float ComputeWindowSeconds(class ASLPlayerRunnerCharacter* Player) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> MainBoxComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> WarningBoxComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Hurdle")
	EHurdleState HurdleState = EHurdleState::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	ERunnerMontageSection RunnerMontageSection = ERunnerMontageSection::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	ERunnerMontageSection FailMontageSection   = ERunnerMontageSection::None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Track")
	TObjectPtr<ALevelSequenceActor> SuccessLevelSequence = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Track")
	TObjectPtr<ALevelSequenceActor> FailLevelSequence = nullptr;
	// 무작위 키 사용 여부 / 후보 키 / 선택 결과
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|QTE")
	bool bRandomizeKey = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|QTE")
	TArray<FString> CandidateKeys = { TEXT("W"), TEXT("A"), TEXT("S"), TEXT("D") };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|QTE")
	FString ExpectedInputKey = "W";
	UPROPERTY(BlueprintReadOnly, Category="Runner|QTE")
	float WindowSec;
	bool bIsUsed = false;
};

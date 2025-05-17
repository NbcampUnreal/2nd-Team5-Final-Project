// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLMinigameTreeSpawnManager.generated.h"

class ASLReactiveObjectTree;
class UBoxComponent;

UCLASS()
class STILLLOADING_API ASLMinigameTreeSpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASLMinigameTreeSpawnManager();

	UFUNCTION(BlueprintImplementableEvent)
	void StartGame();

	UFUNCTION(BlueprintCallable)
	void SetIsPlaying(bool bPlaying) { bIsPlaying = bPlaying; }

	UFUNCTION(BlueprintCallable)
	bool GetIsPlaying() { return bIsPlaying; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void CreateTree(int SpawnCount);

	UFUNCTION(BlueprintCallable)
	void SpawnTree();
	
	UFUNCTION()
	void BeginOverlapTree(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	
public:
	UPROPERTY(BlueprintReadOnly)
	bool bIsPlaying = false;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> SpawnLocationArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UBoxComponent> FinishBoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LineIndex = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BeginSpawnCount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ASLReactiveObjectTree> TreeRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TSet<TObjectPtr<ASLReactiveObjectTree>> TreeSet;
};

// SLSplineTrack.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLSplineTrack.generated.h"

class USplineComponent;
class ALevelSequenceActor;

UCLASS()
class STILLLOADING_API ASLSplineTrack : public AActor
{
	GENERATED_BODY()
public:
	ASLSplineTrack();

	UFUNCTION(BlueprintCallable, Category="Track")
	USplineComponent* GetSplineComp() { return SplineComp; }

	UFUNCTION(BlueprintCallable, Category="Track")
	ASLSplineTrack* GetNextTrack()    { return NextTrack; }

	UFUNCTION(BlueprintCallable, Category="Track")
	ALevelSequenceActor* GetLevelSequenceActor(){ return LevelSequenceActor; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Track")
	TObjectPtr<USplineComponent> SplineComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Track")
	TObjectPtr<ASLSplineTrack> NextTrack = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Track")
	TObjectPtr<ALevelSequenceActor> LevelSequenceActor = nullptr;
};

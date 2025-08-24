// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLSplineTrack.generated.h"

class USplineComponent;

UCLASS()
class STILLLOADING_API ASLSplineTrack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASLSplineTrack();

	UFUNCTION(BlueprintCallable, Category="Track")
	USplineComponent* GetSplineComp() { return SplineComp; }

	UFUNCTION(BlueprintCallable, Category="Track")
	ASLSplineTrack* GetNextTrack() { return NextTrack; }
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Track")
	TObjectPtr<USplineComponent> SplineComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Track")
	TObjectPtr<ASLSplineTrack> NextTrack;
};

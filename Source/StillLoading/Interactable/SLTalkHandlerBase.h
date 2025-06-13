// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTalkHandlerBase.generated.h"

class USLTalkHandlerBase;

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FOnTalkEndSignature, USLTalkHandlerBase, OnTalkEndDelegate, USLTalkHandlerBase*, Component);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLTalkHandlerBase : public UActorComponent
{
	GENERATED_BODY()
	
public:
	USLTalkHandlerBase();
	
	UFUNCTION(BlueprintNativeEvent, Category = "TalkHandler")
	void OnTalkEnd();
	virtual void OnTalkEnd_Implementation();

	UFUNCTION(BlueprintCallable, Category = "TalkHandler")
	FName GetTalkName();
	
	UPROPERTY(BlueprintAssignable, Category = "TalkHandler")
	FOnTalkEndSignature OnTalkEndDelegate;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TalkHandler", meta = (AllowPrivateAccess = "true"))
	FName TalkName;
};

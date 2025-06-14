// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTalkHandlerBase.generated.h"

class USLTalkHandlerBase;

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FOnTalkEndSignature, USLTalkHandlerBase, OnTalkEndDelegate, USLTalkHandlerBase*, Component);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FOnChoiceEndSignature, USLTalkHandlerBase, OnChoiceEndDelegate, bool, bResult);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), DisplayName="대화 핸들러 컴포넌트")
class STILLLOADING_API USLTalkHandlerBase : public UActorComponent
{
	GENERATED_BODY()
	
public:
	USLTalkHandlerBase();
	
	UFUNCTION(BlueprintNativeEvent, Category = "TalkHandler")
	void OnTalkEnd();
	virtual void OnTalkEnd_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "TalkHandler")
	void OnChoiceEnd(bool bResult);
	virtual void OnChoiceEnd_Implementation(bool bResult);

	UFUNCTION(BlueprintCallable, Category = "TalkHandler")
	FName GetTalkName();

	UFUNCTION(BlueprintCallable, Category = "TalkHandler")
	void SetTalkName(const FName InName);
	
	UPROPERTY(BlueprintAssignable, Category = "TalkHandler")
	FOnTalkEndSignature OnTalkEndDelegate;

	UPROPERTY(BlueprintAssignable, Category = "TalkHandler")
	FOnChoiceEndSignature OnChoiceEndDelegate;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TalkHandler", meta = (AllowPrivateAccess = "true"))
	FName TalkName;
};

#pragma once

#include "CoreMinimal.h"
#include "Character/SLAIBaseCharacter.h"
#include "SLStableMasterCharacter.generated.h"

class ASLStableMasterAIController;

UCLASS()
class STILLLOADING_API ASLStableMasterCharacter : public ASLAIBaseCharacter
{
	GENERATED_BODY()

public:
	ASLStableMasterCharacter();

	virtual void OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	void OnBodyCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	virtual void BeginPlay() override;

	// 충돌 관련 블루프린트 이벤트들
	UFUNCTION(BlueprintImplementableEvent, Category = "StableMaster|Collision")
	void OnHideableCharacterCollided(AActor* HideableCharacter);

	UFUNCTION(BlueprintImplementableEvent, Category = "StableMaster|Collision")  
	void OnHidingCharacterCollidedWhileSpotted(AActor* HideableCharacter);

	void HandleHideableCharacterCollision(AActor* HideableCharacter);

	// 일반 충돌 감지 범위 (숨지 않은 상태)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StableMaster|Detection")
	float NormalDetectionRange;

	// 숨어있는 캐릭터 감지 범위 (숨은 상태)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StableMaster|Detection")
	float HidingDetectionRange;

private:
	UPROPERTY()
	TObjectPtr<ASLStableMasterAIController> StableMasterController;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> ProcessedActors;
};
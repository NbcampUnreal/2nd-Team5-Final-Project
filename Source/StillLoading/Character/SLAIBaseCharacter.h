// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLBaseCharacter.h"
#include "SLAIBaseCharacter.generated.h"

class UBoxComponent;
class UBlackboardComponent;
class ASLBaseAIController;

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Moving      UMETA(DisplayName = "Moving"),
	Combat		UMETA(DisplayName = "Combat"),
	Dead        UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	Front,
	Back,
	Left,
	Right
};

UCLASS()
class STILLLOADING_API ASLAIBaseCharacter : public ASLBaseCharacter
{
	GENERATED_BODY()

public:
	ASLAIBaseCharacter();
	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	FORCEINLINE UBoxComponent* GetLeftHandCollisionBox() const { return LeftHandCollisionBox; }
	FORCEINLINE UBoxComponent* GetRightHandCollisionBox() const { return RightHandCollisionBox; }

	UFUNCTION()
	virtual void OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	// 플레이어를 바라보는 상태인지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	bool ShouldLookAtPlayer;

	//공격중인지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	bool IsAttacking;
	
	//맞았을때 반응 할건지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	bool IsHitReaction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	bool IsDead;
	
private:
	// AI 컨트롤러 참조
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ASLBaseAIController> AIController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* LeftHandCollisionBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FName LeftHandCollisionBoxAttachBoneName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightHandCollisionBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FName RightHandCollisionBoxAttachBoneName;
;
};

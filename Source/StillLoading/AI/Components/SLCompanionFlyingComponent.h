#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AI/DataTypes/SLCompanionDataTypes.h"
#include "SLCompanionFlyingComponent.generated.h"

class ASLCompanionCharacter;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFlyingStateChanged, bool, bIsFlying);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLCompanionFlyingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USLCompanionFlyingComponent();

    UFUNCTION(BlueprintCallable, Category = "Flying")
    void StartFlying();
    
    UFUNCTION(BlueprintCallable, Category = "Flying")
    void StopFlying();
    
    UFUNCTION(BlueprintCallable, Category = "Flying")
    void FlyToHeight(float TargetHeight, float Speed = 200);
    
    UFUNCTION(BlueprintCallable, Category = "Flying")
    FORCEINLINE bool IsFlying() const { return bIsFlying; }
    
    UFUNCTION(BlueprintCallable, Category = "Flying")
    FORCEINLINE float GetCurrentHeight() const { return CurrentHeight; }

    UPROPERTY(BlueprintAssignable, Category = "Flying")
    FOnFlyingStateChanged OnFlyingStateChanged;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void UpdateFlyingPosition();

    UFUNCTION()
    void SetFlyingState(bool bNewFlyingState);

    UFUNCTION()
    FVector FindGroundPosition(const FVector& WorldPosition);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flying")
    float DefaultHeight;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flying")
    float MaxHeight;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flying")
    float MinHeight;

private:
    UPROPERTY()
    TObjectPtr<ASLCompanionCharacter> OwnerCharacter;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flying", meta = (AllowPrivateAccess = "true"))
    bool bIsFlying;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flying", meta = (AllowPrivateAccess = "true"))
    float CurrentHeight;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flying", meta = (AllowPrivateAccess = "true"))
    float TargetHeight;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flying", meta = (AllowPrivateAccess = "true"))
    float CurrentSpeed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flying", meta = (AllowPrivateAccess = "true"))
    FTimerHandle FlyingUpdateTimer;
};
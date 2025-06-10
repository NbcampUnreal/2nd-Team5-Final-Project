// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLFlashComponent.generated.h"

UENUM(BlueprintType)
enum class EFlashType : uint8
{
    Visibility     UMETA(DisplayName = "Visibility Toggle"),
    ColorChange    UMETA(DisplayName = "Color Change"),
    Opacity        UMETA(DisplayName = "Opacity Change")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLFlashComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USLFlashComponent();
    
    UFUNCTION(BlueprintCallable, Category = "Flash Component")
    void StartFlashing();

    UFUNCTION(BlueprintCallable, Category = "Flash Component")
    void StopFlashing();

    UFUNCTION(BlueprintCallable, Category = "Flash Component")
    bool IsFlashing() const { return bIsFlashing; }
    
    // 메시 재검색
    UFUNCTION(BlueprintCallable, Category = "Flash Component")
    void RefreshMeshComponents();
protected:
    virtual void BeginPlay() override;

private:
    void SetupMeshComponents();
    void CreateDynamicMaterials();
    void FlashToggle();
    void EndFlashing();
    void RestoreOriginalState();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash Settings")
    EFlashType FlashType = EFlashType::Visibility;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash Settings", meta = (ClampMin = "0.1"))
    float FlashDuration = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash Settings", meta = (ClampMin = "0.01"))
    float FlashSpeed = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash Settings")
    FLinearColor FlashColor = FLinearColor::Red;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float FlashOpacity = 0.3f;
private:
    UPROPERTY()
    TArray<UMeshComponent*> MeshComponents;

    // 각 메시별 원본 머티리얼 저장
    TMap<UMeshComponent*, TArray<UMaterialInterface*>> OriginalMaterialsMap;

    // 각 메시별 다이나믹 머티리얼 저장
    TMap<UMeshComponent*, TArray<UMaterialInstanceDynamic*>> DynamicMaterialsMap;

    FTimerHandle FlashTimerHandle;
    FTimerHandle FlashEndTimerHandle;
    
    bool bIsFlashing = false;
    bool bFlashState = true;
    FString ColorParameterName = TEXT("BaseColor");
    FString OpacityParameterName = TEXT("Opacity");


};
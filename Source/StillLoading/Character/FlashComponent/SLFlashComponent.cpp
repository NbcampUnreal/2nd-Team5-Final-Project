// Fill out your copyright notice in the Description page of Project Settings.


#include "SLFlashComponent.h"
#include "Components/MeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "TimerManager.h"

USLFlashComponent::USLFlashComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}
void USLFlashComponent::BeginPlay()
{
    Super::BeginPlay();
    SetupMeshComponents();
}

void USLFlashComponent::SetupMeshComponents()
{
    MeshComponents.Empty();
    OriginalMaterialsMap.Empty();
    DynamicMaterialsMap.Empty();

    AActor* Owner = GetOwner();
    if (!Owner) return;
    
    // 모든 메시 컴포넌트 찾기
    Owner->GetComponents<UMeshComponent>(MeshComponents);

    // 각 메시의 원본 머티리얼 저장
    for (UMeshComponent* MeshComp : MeshComponents)
    {
        TArray<UMaterialInterface*> OriginalMaterials;
        for (int32 i = 0; i < MeshComp->GetNumMaterials(); i++)
        {
            OriginalMaterials.Add(MeshComp->GetMaterial(i));
        }
        OriginalMaterialsMap.Add(MeshComp, OriginalMaterials);
    }

    // 색상 변경이나 투명도 변경이면 다이나믹 머티리얼 생성
    if (FlashType == EFlashType::ColorChange || FlashType == EFlashType::Opacity)
    {
        CreateDynamicMaterials();
    }

    UE_LOG(LogTemp, Log, TEXT("FlashComponent: Found %d mesh components on %s"), 
           MeshComponents.Num(), *Owner->GetName());
}

void USLFlashComponent::CreateDynamicMaterials()
{
    for (UMeshComponent* MeshComp : MeshComponents)
    {
        TArray<UMaterialInstanceDynamic*> DynamicMaterials;
        for (int32 i = 0; i < MeshComp->GetNumMaterials(); i++)
        {
            UMaterialInstanceDynamic* DynMat = MeshComp->CreateDynamicMaterialInstance(i);
            DynamicMaterials.Add(DynMat);
        }
        DynamicMaterialsMap.Add(MeshComp, DynamicMaterials);
    }
}

void USLFlashComponent::StartFlashing()
{
    if (MeshComponents.Num() == 0 || bIsFlashing) return;

    bIsFlashing = true;
    bFlashState = true;

    // 점멸 토글 타이머 시작
    GetWorld()->GetTimerManager().SetTimer(FlashTimerHandle, this, 
        &USLFlashComponent::FlashToggle, FlashSpeed, true);

    // 점멸 종료 타이머 설정
    GetWorld()->GetTimerManager().SetTimer(FlashEndTimerHandle, this, 
        &USLFlashComponent::EndFlashing, FlashDuration, false);
}

void USLFlashComponent::FlashToggle()
{
    if (MeshComponents.Num() == 0) return;

    bFlashState = !bFlashState;

    for (UMeshComponent* MeshComp : MeshComponents)
    {
        if (!IsValid(MeshComp)) continue;

        switch (FlashType)
        {
            case EFlashType::Visibility:
            {
                MeshComp->SetVisibility(bFlashState);
                break;
            }
            case EFlashType::ColorChange:
            {
                if (DynamicMaterialsMap.Contains(MeshComp))
                {
                    TArray<UMaterialInstanceDynamic*>& DynMaterials = DynamicMaterialsMap[MeshComp];
                    for (UMaterialInstanceDynamic* DynMat : DynMaterials)
                    {
                        if (DynMat)
                        {
                            FLinearColor ColorToSet = bFlashState ? FLinearColor::White : FlashColor;
                            DynMat->SetVectorParameterValue(*ColorParameterName, ColorToSet);
                        }
                    }
                }
                break;
            }
            case EFlashType::Opacity:
            {
                if (DynamicMaterialsMap.Contains(MeshComp))
                {
                    TArray<UMaterialInstanceDynamic*>& DynMaterials = DynamicMaterialsMap[MeshComp];
                    for (UMaterialInstanceDynamic* DynMat : DynMaterials)
                    {
                        if (DynMat)
                        {
                            float OpacityToSet = bFlashState ? 1.0f : FlashOpacity;
                            DynMat->SetScalarParameterValue(*OpacityParameterName, OpacityToSet);
                        }
                    }
                }
                break;
            }
        }
    }
}

void USLFlashComponent::StopFlashing()
{
    if (!bIsFlashing) return;

    bIsFlashing = false;

    // 타이머들 정리
    GetWorld()->GetTimerManager().ClearTimer(FlashTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(FlashEndTimerHandle);

    RestoreOriginalState();
}

void USLFlashComponent::EndFlashing()
{
    StopFlashing();
}

void USLFlashComponent::RestoreOriginalState()
{
    for (UMeshComponent* MeshComp : MeshComponents)
    {
        if (!IsValid(MeshComp)) continue;

        switch (FlashType)
        {
            case EFlashType::Visibility:
            {
                MeshComp->SetVisibility(true);
                break;
            }
            case EFlashType::ColorChange:
            {
                if (DynamicMaterialsMap.Contains(MeshComp))
                {
                    TArray<UMaterialInstanceDynamic*>& DynMaterials = DynamicMaterialsMap[MeshComp];
                    for (UMaterialInstanceDynamic* DynMat : DynMaterials)
                    {
                        if (DynMat)
                        {
                            DynMat->SetVectorParameterValue(*ColorParameterName, FLinearColor::White);
                        }
                    }
                }
                break;
            }
            case EFlashType::Opacity:
            {
                if (DynamicMaterialsMap.Contains(MeshComp))
                {
                    TArray<UMaterialInstanceDynamic*>& DynMaterials = DynamicMaterialsMap[MeshComp];
                    for (UMaterialInstanceDynamic* DynMat : DynMaterials)
                    {
                        if (DynMat)
                        {
                            DynMat->SetScalarParameterValue(*OpacityParameterName, 1.0f);
                        }
                    }
                }
                break;
            }
        }
    }
}

void USLFlashComponent::RefreshMeshComponents()
{
    if (bIsFlashing)
    {
        StopFlashing();
    }
    SetupMeshComponents();
}

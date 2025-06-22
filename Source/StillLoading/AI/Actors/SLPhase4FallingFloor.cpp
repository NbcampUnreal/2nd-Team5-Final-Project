#include "SLPhase4FallingFloor.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

ASLPhase4FallingFloor::ASLPhase4FallingFloor()
{
    PrimaryActorTick.bCanEverTick = false;

    CollapseDelay = 1.0f;
    
    DefaultSafePatterns.Add(TEXT("Stay"));
}

void ASLPhase4FallingFloor::BeginPlay()
{
    Super::BeginPlay();
    
    if (IsValid(GetWorld()))
    {
        FTimerHandle InitTimer;
        GetWorld()->GetTimerManager().SetTimer(
            InitTimer,
            [this]()
            {
                CollectAllMeshComponents();
                
                SetMeshesToSafeByName(DefaultSafePatterns);
            },
            0.1f,  // 0.1초 딜레이
            false
        );
    }
}

void ASLPhase4FallingFloor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    
    CollectAllMeshComponents();
}

void ASLPhase4FallingFloor::StartFloorCollapse()
{
    OnFloorCollapseStarted.Broadcast();
    
    if (IsValid(GetWorld()))
    {
        GetWorld()->GetTimerManager().SetTimer(
            CollapseTimer,
            this,
            &ASLPhase4FallingFloor::OnCollapseDelayFinished,
            CollapseDelay,
            false
        );
    }
}

void ASLPhase4FallingFloor::ResetFloor()
{
    if (IsValid(GetWorld()))
    {
        GetWorld()->GetTimerManager().ClearTimer(CollapseTimer);
    }

    // 모든 메시 물리 비활성화 및 원래 위치로 복구
    for (UStaticMeshComponent* MeshComp : AllMeshComponents)
    {
        if (IsValid(MeshComp))
        {
            MeshComp->SetSimulatePhysics(false);
            MeshComp->SetCollisionObjectType(ECC_WorldStatic);
            
            // 부모에 다시 어태치 (분리되었던 경우)
            if (!MeshComp->IsAttachedTo(RootComponent))
            {
                MeshComp->AttachToComponent(
                    RootComponent,
                    FAttachmentTransformRules::KeepRelativeTransform
                );
            }
        }
    }
}

void ASLPhase4FallingFloor::RefreshMeshList()
{
    CollectAllMeshComponents();
    
    // 목록 출력
    for (int32 i = 0; i < AllMeshComponents.Num(); i++)
    {
        if (IsValid(AllMeshComponents[i]))
        {
            UE_LOG(LogTemp, Display, TEXT("📦 [%d] %s"), i, *AllMeshComponents[i]->GetName());
        }
    }
}

void ASLPhase4FallingFloor::SetAllMeshesToFall()
{
    SafeMeshComponents.Empty();
}

void ASLPhase4FallingFloor::SetAllMeshesToSafe()
{
    SafeMeshComponents = AllMeshComponents;
}

void ASLPhase4FallingFloor::TestStayPattern()
{
    
    TArray<FString> TestPatterns;
    TestPatterns.Add(TEXT("Stay"));
    
    SetMeshesToSafeByName(TestPatterns);
}

void ASLPhase4FallingFloor::ApplyDefaultPatterns()
{
    SetMeshesToSafeByName(DefaultSafePatterns);
}

void ASLPhase4FallingFloor::SetMeshesToSafeByName(const TArray<FString>& SafePatterns)
{
    SafeMeshComponents.Empty();
    
    for (UStaticMeshComponent* MeshComp : AllMeshComponents)
    {
        if (!IsValid(MeshComp))
        {
            continue;
        }
        
        FString MeshName = MeshComp->GetName();
        
        bool bIsSafe = false;
        
        // 패턴 확인
        for (const FString& Pattern : SafePatterns)
        {
            
            if (MeshName.Contains(Pattern))
            {
                bIsSafe = true;
                SafeMeshComponents.Add(MeshComp);
                break;
            }
        }
    }
}

void ASLPhase4FallingFloor::SetMeshesToFallByName(const TArray<FString>& FallPatterns)
{
    
    for (const FString& Pattern : FallPatterns)
    {
        for (int32 i = SafeMeshComponents.Num() - 1; i >= 0; i--)
        {
            if (IsValid(SafeMeshComponents[i]) && SafeMeshComponents[i]->GetName().Contains(Pattern))
            {
                SafeMeshComponents.RemoveAt(i);
            }
        }
    }
}

void ASLPhase4FallingFloor::CollectAllMeshComponents()
{
    AllMeshComponents.Empty();
    
    // 이 액터의 모든 StaticMeshComponent 수집 (하위 컴포넌트 포함)
    TArray<UStaticMeshComponent*> FoundMeshes;
    GetComponents<UStaticMeshComponent>(FoundMeshes, true); // true = 하위 컴포넌트 포함
    
    for (UStaticMeshComponent* MeshComp : FoundMeshes)
    {
        if (IsValid(MeshComp) && MeshComp->GetStaticMesh())
        {
            AllMeshComponents.Add(MeshComp);
        }
    }
}

void ASLPhase4FallingFloor::MakeMeshFall(UStaticMeshComponent* MeshComp)
{
    if (!IsValid(MeshComp))
    {
        return;
    }

    // 부모에서 분리
    MeshComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    
    // 물리 설정
    MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MeshComp->SetCollisionObjectType(ECC_WorldDynamic);
    MeshComp->SetCollisionResponseToAllChannels(ECR_Block);
    MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    
    // 물리 활성화
    MeshComp->SetSimulatePhysics(true);
}

bool ASLPhase4FallingFloor::IsSafeMesh(UStaticMeshComponent* MeshComp) const
{
    return SafeMeshComponents.Contains(MeshComp);
}

void ASLPhase4FallingFloor::OnCollapseDelayFinished()
{
    for (UStaticMeshComponent* MeshComp : AllMeshComponents)
    {
        if (IsValid(MeshComp))
        {
            if (!IsSafeMesh(MeshComp))
            {
                MakeMeshFall(MeshComp);
            }
        }
    }
    
    OnFloorCollapseCompleted.Broadcast();
}
#include "CameraOcclusionHandlerComponent.h"
#include "Components/StaticMeshComponent.h"

UCameraOcclusionHandlerComponent::UCameraOcclusionHandlerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UCameraOcclusionHandlerComponent::BeginPlay()
{
    Super::BeginPlay();

    OnComponentBeginOverlap.AddDynamic(this, &UCameraOcclusionHandlerComponent::OnOcclusionOverlapBegin);
    OnComponentEndOverlap.AddDynamic(this, &UCameraOcclusionHandlerComponent::OnOcclusionOverlapEnd);

    SetCollisionResponseToAllChannels(ECR_Overlap);
    SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SetCollisionObjectType(ECC_WorldDynamic);
}

void UCameraOcclusionHandlerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bShowDebugBox)
    {
        return;
    }

    DrawDebugBox(
        GetWorld(),                       // 월드
        GetComponentLocation(),           // 박스의 월드 위치 (중심점)
        GetScaledBoxExtent(),             // 박스의 크기 (스케일 적용된 절반 크기)
        GetComponentQuat(),               // 박스의 월드 회전
        FColor::Green,                    // 라인 색상
        false,                            // 선이 계속 남을지 여부 (false = 한 프레임만)
        0.0f,                             // 라인이 지속될 시간 (0 = 한 프레임만)
        0,                                // 뎁스 우선순위 (보통 0)
        1.5f                              // 라인 두께
    );
}

void UCameraOcclusionHandlerComponent::OnOcclusionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor == GetOwner() || !OtherComp) return;

    //UE_LOG(LogTemp, Warning, TEXT("Overlap BEGIN - Actor: %s, Component: %s"), *OtherActor->GetName(), *OtherComp->GetName());

    if (UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(OtherComp))
    {
        ApplyTransparencyToMesh(Mesh);
    }
}

void UCameraOcclusionHandlerComponent::OnOcclusionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor == GetOwner() || !OtherComp) return;
    
    //UE_LOG(LogTemp, Warning, TEXT("Overlap END - Actor: %s, Component: %s"), *OtherActor->GetName(), *OtherComp->GetName());
  
    if (UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(OtherComp))
    {
        RestoreMeshMaterial(Mesh);
    }
}

void UCameraOcclusionHandlerComponent::ApplyTransparencyToMesh(UStaticMeshComponent* Mesh)
{
    if (!TransparentMaterial || !Mesh || OriginalComponentMaterials.Contains(Mesh)) return;

    FActorMaterialSet OriginalMats;
    for (int32 i = 0; i < Mesh->GetNumMaterials(); ++i)
    {
        OriginalMats.Materials.Add(Mesh->GetMaterial(i));
    }
    OriginalComponentMaterials.Add(Mesh, OriginalMats);

    for (int32 i = 0; i < Mesh->GetNumMaterials(); ++i)
    {
        Mesh->SetMaterial(i, TransparentMaterial);
    }
}

void UCameraOcclusionHandlerComponent::RestoreMeshMaterial(UStaticMeshComponent* Mesh)
{
    if (!Mesh || !OriginalComponentMaterials.Contains(Mesh)) return;

    const FActorMaterialSet& OriginalMats = OriginalComponentMaterials.FindChecked(Mesh);
    for (int32 i = 0; i < OriginalMats.Materials.Num() && i < Mesh->GetNumMaterials(); ++i)
    {
        if (OriginalMats.Materials.IsValidIndex(i))
        {
            Mesh->SetMaterial(i, OriginalMats.Materials[i]);
        }
    }

    OriginalComponentMaterials.Remove(Mesh);
}
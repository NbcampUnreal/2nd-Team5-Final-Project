#include "CollisionRadarComponent.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "MotionWarpingComponent.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY(LogCollisionRadarComponent);

UCollisionRadarComponent::UCollisionRadarComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    DetectionZone = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionZone"));
    
    DetectionZone->SetGenerateOverlapEvents(true);
    DetectionZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DetectionZone->SetCollisionObjectType(ECC_GameTraceChannel2);
    DetectionZone->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    DetectionZone->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
}

void UCollisionRadarComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!bIsUseRadar || !DetectionZone)
    {
        return;
    }

    DetectionZone->SetSphereRadius(DetectionRadius, true);
    DetectionZone->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    DetectionZone->RegisterComponent();
    
    DetectionZone->OnComponentBeginOverlap.AddDynamic(this, &UCollisionRadarComponent::OnOverlapBegin);
    DetectionZone->OnComponentEndOverlap.AddDynamic(this, &UCollisionRadarComponent::OnOverlapEnd);

    if (AActor* Owner = GetOwner())
    {
        if (!Owner->GetRootComponent())
        {
            Owner->SetRootComponent(DetectionZone);
        }
        else
        {
            DetectionZone->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
        }
    }

    GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle, this, &UCollisionRadarComponent::DetectClosestActorInFOV, UpdateInterval, true);
}

bool UCollisionRadarComponent::IsInFieldOfView(const AActor* TargetActor) const
{
    if (!TargetActor)
        return false;

    //UE_LOG(LogCollisionRadarComponent, Warning, TEXT("IsInFieldOfView::Target Actor: %s"), *TargetActor->GetName());

    FVector MyLocation = GetOwner()->GetActorLocation();
    FVector TargetLocation = TargetActor->GetActorLocation();

    float Distance = FVector::Dist(MyLocation, TargetLocation);
    if (Distance > DetectionRadius)
        return false;

    FVector ForwardVector = GetOwner()->GetActorForwardVector().GetSafeNormal();
    FVector ToTarget = (TargetLocation - MyLocation).GetSafeNormal();

    float DotProduct = FVector::DotProduct(ForwardVector, ToTarget);
    float CosHalfFOV = FMath::Cos(FMath::DegreesToRadians(FieldOfView * 0.5f));

    return DotProduct >= CosHalfFOV;
}

void UCollisionRadarComponent::DetectClosestActorInFOV()
{
    AActor* ClosestActor = nullptr;
    float MinDistance = FLT_MAX;
    FVector MyLocation = GetOwner()->GetActorLocation();

    for (AActor* Actor : NearbyActors)
    {
        if (!Actor || Actor == GetOwner())
            continue;

        if (IsInFieldOfView(Actor))
        {
            float Distance = FVector::Dist(MyLocation, Actor->GetActorLocation());
            //UE_LOG(LogCollisionRadarComponent, Warning, TEXT("Detected Actor in FOV: %s, Distance: %.2f"), *Actor->GetName(), Distance);
            
            if (Distance < MinDistance)
            {
                MinDistance = Distance;
                ClosestActor = Actor;
            }
        }
    }

    if (ClosestActor)
    {
        //TODO: 여기서 딜리게이트로 뿌리던지 상호작용 시작점 케릭터랑 가장 가까운 액터 위치
        //UE_LOG(LogCollisionRadarComponent, Warning, TEXT("Closest Actor in FOV: %s, Distance: %.2f"), *ClosestActor->GetName(), MinDistance);
        FVector TargetLoc = ClosestActor->GetActorLocation() - ClosestActor->GetActorForwardVector()*137.f;
        FRotator TargetRot = ClosestActor->GetActorRotation();
        MotionWarpComponent->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("Warp"), TargetLoc, TargetRot);
    }
    
    DrawDebugVisualization();
}

void UCollisionRadarComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                              bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != GetOwner())
    {
        NearbyActors.AddUnique(OtherActor);

        //UE_LOG(LogCollisionRadarComponent, Warning, TEXT("Actor Overlapped with: %s"), *OtherActor->GetName());
        
        if (IsInFieldOfView(OtherActor))
        {
            FDetectedActorList DetectedActorList;
            FDetectedActorInfo ActorInfo;
            ActorInfo.Actor = OtherActor;
            ActorInfo.Distance = 0.0f;
            
            DetectedActorList.DetectedActors.Add(ActorInfo);
            OnActorDetectedEnhanced.Broadcast(DetectedActorList);
        }
    }
}


void UCollisionRadarComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor != GetOwner())
    {
        NearbyActors.Remove(OtherActor);
    }
}

void UCollisionRadarComponent::DrawDebugVisualization()
{
#if WITH_EDITOR
    if (!bShowDetectionRadius && !bShowFieldOfView)
        return;

    FVector Location = GetOwner()->GetActorLocation();
    FVector Forward = GetOwner()->GetActorForwardVector();

    if (bShowDetectionRadius)
    {
        DrawDebugSphere(GetWorld(), Location, DetectionRadius, 32, FColor::Green, false, 0.1f, 0, 2.0f);
    }

    if (bShowFieldOfView)
    {
        float HalfFOV = FMath::DegreesToRadians(FieldOfView * 0.5f);
        DrawDebugCone(GetWorld(), Location, Forward, DetectionRadius, HalfFOV, HalfFOV, 30, FColor::Red, false, 0.1f, 0, 2.0f);
    }
#endif
}

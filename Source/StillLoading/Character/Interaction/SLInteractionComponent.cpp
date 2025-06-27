// Fill out your copyright notice in the Description page of Project Settings.


#include "SLInteractionComponent.h"

#include "Character/SLPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Interactable/SLInteractableObjectBase.h"


USLInteractionComponent::USLInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	SetGenerateOverlapEvents(true);
	Super::SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Super::SetCollisionObjectType(ECC_GameTraceChannel2);
	Super::SetCollisionResponseToAllChannels(ECR_Ignore);
	Super::SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
}

ASLInteractableObjectBase* USLInteractionComponent::GetInteractableObject() const
{
    TArray<FHitResult> HitResults;
    const FVector StartLocation = GetComponentLocation() + GetOwnerForwardVector() * InteractionOffset;
    const FVector EndLocation = StartLocation;

    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = false;
    QueryParams.AddIgnoredActor(GetOwner());

    GetWorld()->SweepMultiByChannel(
        HitResults,
        StartLocation,
        EndLocation,
        FQuat::Identity,
        ECC_GameTraceChannel2,
        FCollisionShape::MakeSphere(InteractionRadius),
        QueryParams
    );

    if (bIsShowDebugLine)
    {
        DrawDebugLine(GetWorld(), GetComponentLocation(), StartLocation, FColor::Green, false, 2.0f);
        DrawDebugSphere(GetWorld(), StartLocation, InteractionRadius, 12, FColor::Blue, false, 2.0f);
    }

    float ClosestDistance = MAX_FLT;
    ASLInteractableObjectBase* ClosestObject = nullptr;

    for (const FHitResult& Hit : HitResults)
    {
        if (ASLInteractableObjectBase* InteractableObject = Cast<ASLInteractableObjectBase>(Hit.GetActor()))
        {
            const float Distance = FVector::DistSquared(StartLocation, Hit.Location);
            if (Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                ClosestObject = InteractableObject;
            }
        }
    }

    return ClosestObject;
}


void USLInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &USLInteractionComponent::OnInteractionDetectedBegin);
	OnComponentEndOverlap.AddDynamic(this, &USLInteractionComponent::OnInteractionDetectedEnd);

}


#if WITH_EDITOR
void USLInteractionComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	SphereRadius = InteractionDetectRadius;
}
#endif

FVector USLInteractionComponent::GetOwnerForwardVector() const
{
	if (const ASLPlayerCharacter* OwnerActor = Cast<ASLPlayerCharacter>(GetOwner()))
	{
		return bIs2DCharacter ? OwnerActor->GetMesh()->GetRightVector() : OwnerActor->GetActorForwardVector();
	}
	return FVector::ZeroVector;
}

void USLInteractionComponent::OnInteractionDetectedBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASLInteractableObjectBase* InteractableObject = Cast<ASLInteractableObjectBase>(OtherActor))
	{
		InteractableObject->OnDetected();
	}
}

void USLInteractionComponent::OnInteractionDetectedEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (ASLInteractableObjectBase* InteractableObject = Cast<ASLInteractableObjectBase>(OtherActor))
	{
		InteractableObject->OnUndetected();
	}
}




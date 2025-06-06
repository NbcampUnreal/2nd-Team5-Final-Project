
#include "SLAIFixingAxisComponent.h"

#include "GameFramework/CharacterMovementComponent.h"


USLAIFixingAxisComponent::USLAIFixingAxisComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
}

void USLAIFixingAxisComponent::BeginPlay()
{
    Super::BeginPlay();
    UActorComponent* MovementActorComponent = GetOwner()->GetComponentByClass(UCharacterMovementComponent::StaticClass());
    UActorComponent* SkeletalMeshActorComponent = GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass());
    check(MovementActorComponent);
    check(SkeletalMeshActorComponent);
    MovementComponent = Cast<UCharacterMovementComponent>(MovementActorComponent);
    SkeletalMeshComponent = Cast<USkeletalMeshComponent>(SkeletalMeshActorComponent);
    
}

void USLAIFixingAxisComponent::TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    const FVector& Velocity = MovementComponent->GetVelocityForNavMovement();
    if (Velocity.Length() < VelocityThreshold)
    {
        return;
    }
    
    const FVector UnitVelocity = Velocity.GetSafeNormal();

    const float XRotation = FMath::RadiansToDegrees(FMath::Atan2(UnitVelocity.Z, FMath::Sqrt(FMath::Square(UnitVelocity.X) + FMath::Square(UnitVelocity.Y))));
    const float YRotation = FMath::RadiansToDegrees(FMath::Atan2(UnitVelocity.Y, UnitVelocity.X)) - 90.0f;

    UE_LOG(LogTemp, Warning, TEXT("[USLAIFixingAxisComponent::TickComponent] : X  %f, Y : %f "), XRotation, YRotation );

    
    const float RoundedYRotation = FMath::RoundToFloat(YRotation / 90.0f) * 90.0f;
    SkeletalMeshComponent->SetRelativeRotation(FRotator(0.0f, RoundedYRotation, 0.0f));
}


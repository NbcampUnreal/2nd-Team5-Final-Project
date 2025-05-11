#include "SLAIBaseCharacter.h"

#include "AnimInstances/SLAICharacterAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Controller/SLBaseAIController.h"
#include "GameFramework/CharacterMovementComponent.h"


ASLAIBaseCharacter::ASLAIBaseCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 180.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;

	LeftHandCollisionBox = CreateDefaultSubobject<UBoxComponent>("LeftHandCollisionBox");
	LeftHandCollisionBox->SetupAttachment(GetMesh());
	LeftHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);

	RightHandCollisionBox = CreateDefaultSubobject<UBoxComponent>("RightHandCollisionBox");
	RightHandCollisionBox->SetupAttachment(GetMesh());
	RightHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);

	ShouldLookAtPlayer = false;
	IsAttacking = false;
	IsHitReaction = false;
	IsDead = false;
}

void ASLAIBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	IsDead = true;

}

float ASLAIBaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (ActualDamage > 0.0f && DamageCauser)
    {
        FVector AttackerLocation = DamageCauser->GetActorLocation();
        
        FVector DirectionVector = AttackerLocation - GetActorLocation();
        DirectionVector.Normalize();
        
        FVector LocalHitDirection = GetActorTransform().InverseTransformVectorNoScale(DirectionVector);
        
        EHitDirection HitDir;
        float AbsX = FMath::Abs(LocalHitDirection.X);
        float AbsY = FMath::Abs(LocalHitDirection.Y);
        
        if (AbsY > AbsX)
        {
            HitDir = (LocalHitDirection.Y > 0) ? EHitDirection::Right : EHitDirection::Left;
        }
        else
        {
            HitDir = (LocalHitDirection.X > 0) ? EHitDirection::Front : EHitDirection::Back;
        }
        
        
        TObjectPtr<USLAICharacterAnimInstance> AnimInstance = Cast<USLAICharacterAnimInstance>(GetMesh()->GetAnimInstance());
        if (AnimInstance && !IsHitReaction)
        {
            AnimInstance->SetHitDirection(HitDir);
            
            AnimInstance->SetIsHit(true);   
        }
    }
    
    return ActualDamage;
}

void ASLAIBaseCharacter::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		// 오버랩 되면 데미지 입힐거임
	}
}



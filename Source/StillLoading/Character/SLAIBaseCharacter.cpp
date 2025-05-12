#include "SLAIBaseCharacter.h"

#include "BrainComponent.h"
#include "AnimInstances/SLAICharacterAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
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
	
	IsHitReaction = false;
	IsDead = false;
	MaxHealth = 100.0f;
}

void ASLAIBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	AIController = Cast<ASLBaseAIController>(GetController());
	AnimInstancePtr = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
}

float ASLAIBaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (ActualDamage > 0.0f)
    {
        CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

        if (CurrentHealth <= 0.0f)
        {
            if (!IsDead)
            {
                IsDead = true;

                if (AIController)
                {
                    AIController->GetBrainComponent()->StopLogic("Character Died"); //AI 로직 중지
                }

                GetCharacterMovement()->StopMovementImmediately();
                GetCharacterMovement()->DisableMovement();
            	
                GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); 

                if (AnimInstancePtr)
                {
                    if (TObjectPtr<USLAICharacterAnimInstance> SLAIAnimInstance = Cast<USLAICharacterAnimInstance>(AnimInstancePtr.Get()))
                    {
                        SLAIAnimInstance->SetIsDead(IsDead);
                    }
                	
                    if (DeathMontages.Num() > 0)
                    {
                        const int32 MontageIndex = FMath::RandRange(0, DeathMontages.Num() - 1);
                        TObjectPtr<UAnimMontage> MontageToPlay = DeathMontages[MontageIndex];
                        if (MontageToPlay)
                        {
                            AnimInstancePtr->Montage_Play(MontageToPlay);
                        }
                    }
                }
            }
        }
        else if (DamageCauser && IsHitReaction && AnimInstancePtr)
        {
            if (TObjectPtr<USLAICharacterAnimInstance> SLAIAnimInstance = Cast<USLAICharacterAnimInstance>(AnimInstancePtr.Get()))
            {
                 FVector AttackerLocation = DamageCauser->GetActorLocation();
                 FVector DirectionVector = AttackerLocation - GetActorLocation(); // 캐릭터 -> 공격자
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
                 
                 SLAIAnimInstance->SetHitDirection(HitDir);
                 SLAIAnimInstance->SetIsHit(true);
            }
        }
    }
    return ActualDamage;
}

void ASLAIBaseCharacter::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (TObjectPtr<APawn> HitPawn = Cast<APawn>(OtherActor))
	{
		// 오버랩 되면 데미지 입힐거임
	}
}
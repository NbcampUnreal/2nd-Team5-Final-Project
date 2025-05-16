#include "SLAIBaseCharacter.h"

#include "BrainComponent.h"
#include "MotionWarpingComponent.h"
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

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("MotionWarpingComponent");
	
}

void ASLAIBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	//CurrentHealth = MaxHealth;
	AIController = Cast<ASLBaseAIController>(GetController());
	AnimInstancePtr = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;

	IsHitReaction = false;
	IsDead = false;
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	CombatPhase = ECombatPhase::ECP_Phase_None;
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
            	
                GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); 

            	GetCharacterMovement()->StopMovementImmediately();
            	GetCharacterMovement()->DisableMovement();
            	
            	if (AIController)
            	{
            		AIController->GetBrainComponent()->StopLogic("Character Died"); 
            	}
            	
            	if (AnimInstancePtr)
            	{
            		if (USLAICharacterAnimInstance* SLAIAnimInstance = Cast<USLAICharacterAnimInstance>(AnimInstancePtr.Get()))
            		{
            			SLAIAnimInstance->SetIsDead(IsDead);
            			SLAIAnimInstance->SetIsHit(false);  
            			SLAIAnimInstance->SetIsDown(false);
            			SLAIAnimInstance->SetIsStun(false);
            			SLAIAnimInstance->SetIsAttacking(false);
            			SLAIAnimInstance->SetShouldLookAtPlayer(false);
            		}
                	
            		if (DeathMontages.Num() > 0)
            		{
            			const int32 MontageIndex = FMath::RandRange(0, DeathMontages.Num() - 1);
            			UAnimMontage* MontageToPlay = DeathMontages[MontageIndex];
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
            if (USLAICharacterAnimInstance* SLAIAnimInstance = Cast<USLAICharacterAnimInstance>(AnimInstancePtr.Get()))
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
                     HitDir = (LocalHitDirection.Y > 0) ? EHitDirection::EHD_Right : EHitDirection::EHD_Left;
                 }
                 else 
                 {
                     HitDir = (LocalHitDirection.X > 0) ? EHitDirection::EHD_Front : EHitDirection::EHD_Back;
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

void ASLAIBaseCharacter::SetCurrentHealth(float NewHealth)
{
	CurrentHealth = NewHealth;
}

void ASLAIBaseCharacter::SetIsHitReaction(bool bNewIsHitReaction)
{
	IsHitReaction = bNewIsHitReaction;
}

void ASLAIBaseCharacter::SetAttackPower(float NewAttackPower)
{
	AttackPower = NewAttackPower;
}

void ASLAIBaseCharacter::SetCombatPhase(ECombatPhase NewCombatPhase)
{
	CombatPhase = NewCombatPhase;
}

ECombatPhase ASLAIBaseCharacter::GetCombatPhase()
{
	return CombatPhase;
}

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

	LeftFootCollisionBox = CreateDefaultSubobject<UBoxComponent>("LeftFootCollisionBox");
	LeftFootCollisionBox->SetupAttachment(GetMesh());
	LeftFootCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftFootCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);

	RightFootCollisionBox = CreateDefaultSubobject<UBoxComponent>("RightFootCollisionBox");
	RightFootCollisionBox->SetupAttachment(GetMesh());
	RightFootCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightFootCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);
	
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
	
	if (OtherActor != this && Cast<ACharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("캐릭터와 오버랩됨: %s"), *OtherActor->GetName());
        
		if (TObjectPtr<APawn> HitPawn = Cast<APawn>(OtherActor))
		{
			// 오버랩 되면 데미지 입힐거임
		}
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

void ASLAIBaseCharacter::ToggleCollision(EToggleDamageType DamageType, bool bEnableCollision)
{
	switch (DamageType)
	{
	case EToggleDamageType::ETDT_LeftHand:
		ToggleLeftHandCollision(bEnableCollision);
		break;
	case EToggleDamageType::ETDT_RightHand:
		ToggleRightHandCollision(bEnableCollision);
		break;
	case EToggleDamageType::ETDT_CurrentEquippedWeapon:
		if (CurrentWeaponCollision)
		{
			CurrentWeaponCollision->SetCollisionEnabled(bEnableCollision ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
		}
		break;
	default:
		break;
	}
}

void ASLAIBaseCharacter::ToggleLeftHandCollision(bool bEnableCollision)
{
	if (LeftHandCollisionBox)
	{
		LeftHandCollisionBox->SetCollisionEnabled(bEnableCollision ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}
}

void ASLAIBaseCharacter::ToggleRightHandCollision(bool bEnableCollision)
{
	if (RightHandCollisionBox)
	{
		RightHandCollisionBox->SetCollisionEnabled(bEnableCollision ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}
}

void ASLAIBaseCharacter::ToggleLeftFootCollision(bool bEnableCollision)
{
	if (LeftFootCollisionBox)
	{
		LeftFootCollisionBox->SetCollisionEnabled(bEnableCollision ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}
}

void ASLAIBaseCharacter::ToggleRightFootCollision(bool bEnableCollision)
{
	if (RightFootCollisionBox)
	{
		RightFootCollisionBox->SetCollisionEnabled(bEnableCollision ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}
}

void ASLAIBaseCharacter::EquipWeapon(AActor* WeaponActor)
{
	if (!WeaponActor)
		return;
    
	// 현재 장착된 무기가 있다면 해제
	UnequipWeapon();
    
	EquippedWeapon = WeaponActor;
    
	// 무기를 캐릭터 메시에 부착
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	WeaponActor->AttachToComponent(GetMesh(), AttachRules, WeaponSocketName);
    
	// 무기 충돌 컴포넌트 찾기 - 무기에 "WeaponCollision"로 태그된 컴포넌트가 있어야함 [콜리전 컴포넌트를 반환하는걸로 함수 만들면 될듯]
	TArray<UPrimitiveComponent*> Components;
	WeaponActor->GetComponents<UPrimitiveComponent>(Components);
    
	for (UPrimitiveComponent* Component : Components)
	{
		if (Component->ComponentHasTag(FName("WeaponCollision")))
		{
			CurrentWeaponCollision = Component;
			Component->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 초기에는 비활성화
			Component->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);
			break;
		}
	}
}

void ASLAIBaseCharacter::UnequipWeapon()
{
	if (EquippedWeapon)
	{
		if (CurrentWeaponCollision)
		{
			CurrentWeaponCollision->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);
			CurrentWeaponCollision = nullptr;
		}
        
		// 캐릭터에서 무기 분리
		EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		EquippedWeapon = nullptr;
	}
}

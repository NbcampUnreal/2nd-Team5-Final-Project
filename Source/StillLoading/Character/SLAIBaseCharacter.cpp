#include "SLAIBaseCharacter.h"

#include "BrainComponent.h"
#include "MotionWarpingComponent.h"
#include "NiagaraComponent.h"
#include "AI/SLAIProjectile.h"
#include "AnimInstances/SLAICharacterAnimInstance.h"
#include "BattleComponent/BattleComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Controller/SLBaseAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


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
    LeftHandCollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);

    RightHandCollisionBox = CreateDefaultSubobject<UBoxComponent>("RightHandCollisionBox");
    RightHandCollisionBox->SetupAttachment(GetMesh());
    RightHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RightHandCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);
    RightHandCollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);

    LeftFootCollisionBox = CreateDefaultSubobject<UBoxComponent>("LeftFootCollisionBox");
    LeftFootCollisionBox->SetupAttachment(GetMesh());
    LeftFootCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    LeftFootCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);
    LeftFootCollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);

    RightFootCollisionBox = CreateDefaultSubobject<UBoxComponent>("RightFootCollisionBox");
    RightFootCollisionBox->SetupAttachment(GetMesh());
    RightFootCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RightFootCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);
    RightFootCollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
    
    MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("MotionWarpingComponent");
    BattleComponent = CreateDefaultSubobject<UBattleComponent>("BattleComponent");
    BattleComponent->OnCharacterHited.AddDynamic(this, &ThisClass::CharacterHit);
    
    // 카메라 채널 무시
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

    // BattleComponent 에서 사용 하기위한 캡슐 셋팅
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
    
    AIChapter = EChapter::EC_None;
    IsDebugMode = false;
}

void ASLAIBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	//CurrentHealth = MaxHealth;
	AIController = Cast<ASLBaseAIController>(GetController());
	AnimInstancePtr = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;

	if (!LeftHandCollisionBoxAttachBoneName.IsNone())
	{
		LeftHandCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftHandCollisionBoxAttachBoneName);
	}
    
	if (!RightHandCollisionBoxAttachBoneName.IsNone())
	{
		RightHandCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandCollisionBoxAttachBoneName);
	}
    
	if (!LeftFootCollisionBoxAttachBoneName.IsNone())
	{
		LeftFootCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftFootCollisionBoxAttachBoneName);
	}
    
	if (!RightFootCollisionBoxAttachBoneName.IsNone())
	{
		RightFootCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightFootCollisionBoxAttachBoneName);
	}

	
	IsHitReaction = false;
	IsDead = false;
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	CombatPhase = ECombatPhase::ECP_Phase_None;
}

void ASLAIBaseCharacter::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	// 자기 자신과 충돌 무시
	if (!OtherActor || OtherActor == this)
	{
		return;
	}
    
	// 캐릭터인지 확인
	ACharacter* HitCharacter = Cast<ACharacter>(OtherActor);
	if (!HitCharacter)
	{
		return;
	}
     
	// 타겟 액터에 BattleComponent가 있는지 확인
	UBattleComponent* TargetBattleComp = OtherActor->FindComponentByClass<UBattleComponent>();
	if (!TargetBattleComp)
	{
		// BattleComponent가 없는 액터는 데미지를 받을 수 없음
		UE_LOG(LogTemp, Warning, TEXT("%s에 BattleComponent가 없어 데미지를 처리할 수 없습니다"), *OtherActor->GetName());
		return;
	}
    
	// 자신의 BattleComponent 확인
	if (!BattleComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("공격자(%s)에 BattleComponent가 없습니다"), *GetName());
		return;
	}

	if (IsDebugMode)
	{
		// 히트 위치 가져오기
		FVector HitLocation = SweepResult.bBlockingHit ? SweepResult.ImpactPoint : SweepResult.Location;
	
		// 히트 위치가 유효하지 않으면 오버랩된 컴포넌트의 위치 사용
		if (HitLocation.IsZero())
		{
			HitLocation = OverlappedComponent->GetComponentLocation();
		}

		// 오버랩된 컴포넌트의 크기에 맞는 디버그 박스 그리기
		if (UBoxComponent* BoxComp = Cast<UBoxComponent>(OverlappedComponent))
		{
			FVector BoxExtent = BoxComp->GetScaledBoxExtent();
			FVector BoxCenter = BoxComp->GetComponentLocation();
			FRotator BoxRotation = BoxComp->GetComponentRotation();
		
			DrawDebugBox(
				GetWorld(),
				BoxCenter,
				BoxExtent,
				BoxRotation.Quaternion(),
				FColor::Red,
				false,
				5.0f,
				0,
				2.0f
			);
		}
	}
	
	// BattleComponent를 통해 데미지 전달
	BattleComponent->SendHitResult(OtherActor, SweepResult, CurrentAttackType);
    
	//히트 이펙트 재생
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
	case EToggleDamageType::ETDT_LeftFoot:
		ToggleLeftFootCollision(bEnableCollision);
		break;
	case EToggleDamageType::ETDT_RightFoot:
		ToggleRightFootCollision(bEnableCollision);
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
	{
		return;
	}
    
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

UBattleComponent* ASLAIBaseCharacter::GetBattleComponent()
{
	return BattleComponent;
}

void ASLAIBaseCharacter::CharacterHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EAttackAnimType AnimType)
{
	// 이미 처형 중이면 무시
	if (bIsBeingExecuted)
	{
		return;
	}
    
	// 처형 공격인지 확인
	if (AnimType == EAttackAnimType::AAT_FinalAttackA || 
		AnimType == EAttackAnimType::AAT_FinalAttackB || 
		AnimType == EAttackAnimType::AAT_FinalAttackC)
	{
		if (CanBeExecuted())
		{
			bIsBeingExecuted = true;  // 처형 상태로 설정
			PlayExecutionAnimation(AnimType, DamageCauser);
			return;
		}
	}
	
	// 데미지 적용
    CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

    if (CurrentHealth <= 0.0f)
    {
        if (!IsDead)
        {
            IsDead = true;
        	
            // 충돌 비활성화
            GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); 

            // 이동 중지
            GetCharacterMovement()->StopMovementImmediately();
            GetCharacterMovement()->DisableMovement();
            
            // AI 컨트롤러 중지
            if (AIController)
            {
                //AIController->GetBrainComponent()->StopLogic("Character Died");

            	if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
            	{
            		BlackboardComponent->SetValueAsBool(FName("Isdead"), true);
            	}
            }
            
            // 애니메이션 설정
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
            }
        }
    }
    else if (DamageCauser && IsHitReaction && AnimInstancePtr)
    {
        // 히트 반응 애니메이션 설정
        if (USLAICharacterAnimInstance* SLAIAnimInstance = Cast<USLAICharacterAnimInstance>(AnimInstancePtr.Get()))
        {
            // 히트 방향 계산
            FVector AttackerLocation = DamageCauser->GetActorLocation();
            FVector DirectionVector = AttackerLocation - GetActorLocation(); // 캐릭터 -> 공격자
            DirectionVector.Normalize();
            
            FVector LocalHitDirection = GetActorTransform().InverseTransformVectorNoScale(DirectionVector);

            // 히트 방향 결정
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
            
            // 애니메이션 인스턴스에 히트 정보 설정
            SLAIAnimInstance->SetHitDirection(HitDir);
            SLAIAnimInstance->SetIsHit(true);
        }
    }

	if (HitEffectComponent)
	{
		HitEffectComponent->SetWorldLocation(HitResult.Location);
		HitEffectComponent->ActivateSystem();
	}
}

void ASLAIBaseCharacter::SetCurrentAttackType(EAttackAnimType NewCurrentAttackType)
{
	CurrentAttackType = NewCurrentAttackType;
}

EAttackAnimType ASLAIBaseCharacter::GetCurrentAttackType()
{
	return CurrentAttackType;
}

EChapter ASLAIBaseCharacter::GetChapter() const
{
	return AIChapter;
}

ASLAIProjectile* ASLAIBaseCharacter::SpawnProjectileAtLocation(TSubclassOf<ASLAIProjectile> ProjectileClass,
	FVector TargetLocation, FName SocketName, float ProjectileSpeed, EAttackAnimType AnimType)
{
	
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnProjectileAtLocation: No projectile class specified"));
		return nullptr;
	}

	// 스폰 위치 계산
	FVector SpawnLocation = GetMesh()->GetSocketLocation(SocketName);
	FRotator SpawnRotation = CalculateProjectileRotation(SpawnLocation, TargetLocation);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	ASLAIProjectile* SpawnedProjectile = GetWorld()->SpawnActor<ASLAIProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (SpawnedProjectile)
	{
		// 프로젝타일 설정
		SpawnedProjectile->SetupSpawnedProjectile(AnimType, ProjectileSpeed);

		// 프로젝타일 속도 설정
		if (SpawnedProjectile->GetProjectileMovement())
		{
			// 타겟 방향으로 발사
			FVector LaunchDirection = (TargetLocation - SpawnLocation).GetSafeNormal();
			SpawnedProjectile->GetProjectileMovement()->Velocity = LaunchDirection * ProjectileSpeed;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn projectile"));
	}

	return SpawnedProjectile;
}

FRotator ASLAIBaseCharacter::CalculateProjectileRotation(const FVector& StartLocation, const FVector& TargetLocation) const
{
	FVector Direction = (TargetLocation - StartLocation).GetSafeNormal();
	return UKismetMathLibrary::MakeRotFromX(Direction);
}

TArray<ASLAIProjectile*> ASLAIBaseCharacter::SpawnProjectileFanAtLocation(TSubclassOf<ASLAIProjectile> ProjectileClass,FVector TargetLocation, FName SocketName, int32 ProjectileCount, float FanHalfAngle, float ProjectileSpeed,EAttackAnimType AnimType)
{
	TArray<ASLAIProjectile*> SpawnedProjectiles;

    if (!ProjectileClass || ProjectileCount <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnProjectileFanAtLocation: Invalid parameters"));
        return SpawnedProjectiles;
    }

    // 스폰 위치 계산
    FVector SpawnLocation;
    if (SocketName != NAME_None && GetMesh() && GetMesh()->DoesSocketExist(SocketName))
    {
        SpawnLocation = GetMesh()->GetSocketLocation(SocketName);
    }
    else
    {
        SpawnLocation = GetActorLocation() + GetActorForwardVector() * 50.0f;
        SpawnLocation.Z += 80.0f;
    }

    // 타겟 방향 계산
    FVector BaseDirection = (TargetLocation - SpawnLocation).GetSafeNormal();

    // 수평 부채꼴 방향들 생성
    TArray<FVector> FanDirections = GenerateHorizontalFanDirections(BaseDirection, ProjectileCount, FanHalfAngle);

    UE_LOG(LogTemp, Display, TEXT("Spawning fan at location: %s with %d projectiles, angle: %f degrees"), 
           *TargetLocation.ToString(), ProjectileCount, FanHalfAngle * 2.0f);

    // 각 방향으로 프로젝타일 스폰
    for (int32 i = 0; i < FanDirections.Num(); i++)
    {
        FVector CurrentDirection = FanDirections[i];
        FRotator SpawnRotation = CurrentDirection.Rotation();

        // 겹침 방지를 위한 작은 랜덤 오프셋
        FVector RandomOffset = FVector(
            FMath::RandRange(-8.0f, 8.0f),
            FMath::RandRange(-8.0f, 8.0f),
            FMath::RandRange(-3.0f, 3.0f)
        );
        
        FVector AdjustedSpawnLocation = SpawnLocation + RandomOffset;

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = this;
        SpawnParams.bNoFail = true;

        ASLAIProjectile* SpawnedProjectile = GetWorld()->SpawnActor<ASLAIProjectile>(
            ProjectileClass, AdjustedSpawnLocation, SpawnRotation, SpawnParams);

        if (SpawnedProjectile)
        {
            SpawnedProjectile->SetupSpawnedProjectile(AnimType, ProjectileSpeed);

            if (SpawnedProjectile->GetProjectileMovement())
            {
                SpawnedProjectile->GetProjectileMovement()->Velocity = CurrentDirection * ProjectileSpeed;
                SpawnedProjectile->GetProjectileMovement()->MaxSpeed = ProjectileSpeed * 1.2f;
            }

            SpawnedProjectiles.Add(SpawnedProjectile);
            
            UE_LOG(LogTemp, Display, TEXT("Spawned fan projectile %d with direction %s"), 
                   i, *CurrentDirection.ToString());
        }
    }

    return SpawnedProjectiles;
}

float ASLAIBaseCharacter::GetHealthPercentage() const
{
	if (MaxHealth <= 0.0f)
	{
		UE_LOG(LogBattleComponent, Warning, TEXT("GetHealthPercentage: MaxHealth is zero or negative (%f)"), MaxHealth);
		return 0.0f;
	}

	float ClampedCurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
    
	float HealthPercentage = (ClampedCurrentHealth / MaxHealth) * 100.0f;
    
	return HealthPercentage;
}

bool ASLAIBaseCharacter::GetIsTargetClose(float DistanceThreshold)
{
	if (AIController)
	{
		if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
		{
			if(ACharacter* TargetCharacter = Cast<ACharacter>(BlackboardComponent->GetValueAsObject(FName("TargetActor"))))
			{
				return GetDistanceTo(TargetCharacter) < DistanceThreshold;
			}
		}
	}
	return false;
}

bool ASLAIBaseCharacter::CanBeExecuted() const
{
	//처형이 가능한 캐릭터인지 확인
	if (!bCanBeExecuted)
	{
		return false;
	}
    
	// 체력이 10% 이하인지 확인
	float HealthPercentage = GetHealthPercentage();
	//return HealthPercentage <= 10.0f && !IsDead;
	return !IsDead;
}

void ASLAIBaseCharacter::PlayExecutionAnimation(EAttackAnimType ExecutionType, AActor* Executor)
{
    UE_LOG(LogTemp, Warning, TEXT("PlayExecutionAnimation Called - ExecutionType: %s"), *UEnum::GetValueAsString(ExecutionType));
    
    if (!AnimInstancePtr)
    {
        UE_LOG(LogTemp, Error, TEXT("AnimInstancePtr is null"));
        return;
    }
    
    if (IsDead)
    {
        UE_LOG(LogTemp, Error, TEXT("Character is already dead"));
        return;
    }
    
    if (!Executor)
    {
        UE_LOG(LogTemp, Error, TEXT("Executor is null"));
        return;
    }
    
    // 처형 애니메이션 몽타주 찾기
    if (UAnimMontage* ExecutionMontage = ExecutionMontages.FindRef(ExecutionType))
    {
        UE_LOG(LogTemp, Warning, TEXT("Found Execution Montage: %s"), *ExecutionMontage->GetName());
        
        // 현재 재생 중인 몽타주 확인
        if (AnimInstancePtr->IsAnyMontagePlaying())
        {
            UE_LOG(LogTemp, Warning, TEXT("Another montage is playing, stopping it"));
            AnimInstancePtr->StopAllMontages(0.0f);
        }
        
        // 이동 중지
        GetCharacterMovement()->StopMovementImmediately();
        GetCharacterMovement()->DisableMovement();
        
        // AI 일시 정지
    	if (AIController)
    	{
    		// 모든 행동 중지
    		AIController->GetBrainComponent()->StopLogic("Execution");
    		AIController->StopMovement();
    		AIController->ClearFocus(EAIFocusPriority::Gameplay);
        
    		// 블랙보드 업데이트
    		if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
    		{
    			BlackboardComponent->SetValueAsBool(FName("IsBeingExecuted"), true);
    			//BlackboardComponent->SetValueAsObject(FName("TargetActor"), nullptr);
    		}
    	}
        
        // 플레이어 방향으로 회전
        FVector DirectionToExecutor = (Executor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        DirectionToExecutor.Z = 0.0f;
        FRotator NewRotation = FRotationMatrix::MakeFromX(DirectionToExecutor).Rotator();
        SetActorRotation(NewRotation);
        
        // 애니메이션 인스턴스 타입 확인
        if (USLAICharacterAnimInstance* SLAIAnimInstance = Cast<USLAICharacterAnimInstance>(AnimInstancePtr.Get()))
        {
            // 히트 반응 등 다른 애니메이션 상태 초기화
            SLAIAnimInstance->SetIsHit(false);
            SLAIAnimInstance->SetIsDown(false);
            SLAIAnimInstance->SetIsStun(false);
            SLAIAnimInstance->SetIsAttacking(false);
            
            UE_LOG(LogTemp, Warning, TEXT("Reset animation states"));
        }
        
        // 처형 애니메이션 재생
        float PlayRate = 1.0f;
        float StartPosition = 0.0f;
        FName StartSection = NAME_None;
        
        float MontageLength = AnimInstancePtr->Montage_Play(ExecutionMontage, PlayRate, EMontagePlayReturnType::MontageLength, StartPosition);
        
        UE_LOG(LogTemp, Warning, TEXT("Montage_Play returned length: %f"), MontageLength);
        
        if (MontageLength > 0.0f)
        {
            UE_LOG(LogTemp, Warning, TEXT("Montage started successfully"));
            
            // 처형 후 사망 처리
            FTimerHandle ExecutionTimer;
            GetWorld()->GetTimerManager().SetTimer(ExecutionTimer, [this]()
            {
                // 처형 완료 후 직접 사망 처리
                CurrentHealth = 0.0f;
                IsDead = true;
                
                // 충돌 비활성화
                GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
                
                // 애니메이션 상태 업데이트
                if (USLAICharacterAnimInstance* SLAIAnimInstance = Cast<USLAICharacterAnimInstance>(AnimInstancePtr.Get()))
                {
                    SLAIAnimInstance->SetIsDead(true);
                }
                
                // AI 컨트롤러 블랙보드 업데이트
                if (AIController)
                {
                    if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
                    {
                        BlackboardComponent->SetValueAsBool(FName("Isdead"), true);
                    }
                }
                
                UE_LOG(LogTemp, Warning, TEXT("Execution completed - Character is now dead"));
                
            }, MontageLength * 0.8f, false);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to play montage"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No Execution Montage found for type: %s"), *UEnum::GetValueAsString(ExecutionType));
    }
}

#if WITH_EDITOR
void ASLAIBaseCharacter::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, LeftHandCollisionBoxAttachBoneName))
	{
		LeftHandCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftHandCollisionBoxAttachBoneName);
	}

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, RightHandCollisionBoxAttachBoneName))
	{
		RightHandCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandCollisionBoxAttachBoneName);
	}

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, LeftFootCollisionBoxAttachBoneName))
	{
		LeftFootCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftFootCollisionBoxAttachBoneName);
	}

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, RightFootCollisionBoxAttachBoneName))
	{
		RightFootCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightFootCollisionBoxAttachBoneName);
	}
}
#endif

TArray<FVector> ASLAIBaseCharacter::GenerateHorizontalFanDirections(const FVector& BaseDirection, int32 Count, float FanHalfAngle) const
{
	TArray<FVector> Directions;
    
	if (Count <= 0)
	{
		return Directions;
	}

	// 기본 방향을 수평면에 투영
	FVector HorizontalBase = FVector(BaseDirection.X, BaseDirection.Y, 0.0f);
	HorizontalBase.Normalize();

	// 기본 방향의 Yaw 각도 계산
	float BaseYaw = FMath::Atan2(HorizontalBase.Y, HorizontalBase.X);

	if (Count == 1)
	{
		// 1개인 경우 기본 방향으로 발사 (원래 Z값 유지)
		Directions.Add(BaseDirection);
		return Directions;
	}

	// 부채꼴 각도 범위에서 균등 분포
	float TotalAngle = FanHalfAngle * 2.0f; // 전체 부채꼴 각도
	float AngleStep = TotalAngle / (Count - 1); // 각 프로젝타일 간 각도

	for (int32 i = 0; i < Count; i++)
	{
		// -FanHalfAngle부터 +FanHalfAngle까지 균등 분포
		float OffsetAngle = -FanHalfAngle + (AngleStep * i);
		float FinalYaw = BaseYaw + FMath::DegreesToRadians(OffsetAngle);

		// 수평 방향 벡터 생성
		FVector Direction = FVector(
			FMath::Cos(FinalYaw),
			FMath::Sin(FinalYaw),
			BaseDirection.Z  // 원래 기본 방향의 Z값 유지
		);

		Direction.Normalize();
		Directions.Add(Direction);

		UE_LOG(LogTemp, Display, TEXT("Fan Direction %d: %s (offset: %.1f°)"), 
			   i, *Direction.ToString(), OffsetAngle);
	}

	return Directions;
}


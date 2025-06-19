#include "SLPlayerCharacter.h"

#include "BattleComponent/BattleComponent.h"
#include "Camera/CameraComponent.h"
#include "CombatHandlerComponent/CombatHandlerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GamePlayTag/GamePlayTag.h"
#include "Item/SLDefaultSword.h"
#include "Item/SLItem.h"
#include "MontageComponent/AnimationMontageComponent.h"
#include "MovementHandlerComponent/SL25DMovementHandlerComponent.h"
#include "MovementHandlerComponent/SLMovementHandlerComponent.h"
#include "PlayerState/SLBattlePlayerState.h"

ASLPlayerCharacter::ASLPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	//bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true; // Zelda-like
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true; // 자체 회전 제어
	//CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 3.f;

	// Spring Arm Collision
	CameraBoom->bDoCollisionTest = true;
	CameraBoom->ProbeChannel = ECC_Camera;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom);
}

void ASLPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (SwordClass)
	{
		Sword = GetWorld()->SpawnActor<ASLItem>(SwordClass, GetActorLocation(), GetActorRotation());
		AttachItemToHand(Sword, TEXT("r_weapon_socket"));
		Sword->SetOwner(this);
	}

	if (ShieldClass)
	{
		Shield = GetWorld()->SpawnActor<ASLItem>(ShieldClass, GetActorLocation(), GetActorRotation());
		AttachItemToHand(Shield, TEXT("l_weapon_socket"));
		Shield->SetOwner(this);
	}

	SetPrimaryState(TAG_Character_Movement_Idle);

	PrintPrimaryStateTags();

	if (UCombatHandlerComponent* CombatHandler = FindComponentByClass<UCombatHandlerComponent>())
	{
		CombatHandler->OnEmpoweredStateChanged.AddDynamic(this, &ASLPlayerCharacter::OnEmpoweredStateChanged);
	}

	CachedMontageComponent = FindComponentByClass<UAnimationMontageComponent>();

	if (GetController())
	{
		if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			if (ASLBattlePlayerState* BasePlayerState = PlayerController->GetPlayerState<ASLBattlePlayerState>())
			{
				CashedPlayerState = BasePlayerState;
				BasePlayerState->OnPlayerHpChanged.AddDynamic(this, &ASLPlayerCharacter::OnPlayerHpChanged);
			}
		}
	}
}

void ASLPlayerCharacter::SwordFromSky()
{
	if (CachedMontageComponent)
	{
		CachedMontageComponent->PlaySkillMontage("SwordFromSky");
	}
}

void ASLPlayerCharacter::CharacterDragged(const bool bIsDragged)
{
	if (!bIsDragged)
	{
		SetPrimaryState(TAG_Character_Movement_Idle);
		if (CachedMontageComponent)
		{
			CachedMontageComponent->GetAnimInstance()->Montage_Stop(0.25f);
		}
	}
	else
	{
		SetPrimaryState(TAG_Character_EnterCinematic);
		if (CachedMontageComponent)
		{
			CachedMontageComponent->PlayTrickMontage("Dragged");
		}
	}
}

void ASLPlayerCharacter::EnterCinematic(const float Yaw)
{
	if (CachedMontageComponent)
	{
		CachedMontageComponent->StopAllMontages(0.2);
	}

	if (USL25DMovementHandlerComponent* CombatHandler = FindComponentByClass<USL25DMovementHandlerComponent>())
	{
		CombatHandler->CachedSkeletalMesh->SetRelativeRotation(FRotator(0.0f, Yaw, 0.0f));
		CombatHandler->SetComponentTickEnabled(false);
	}
	
	SetPrimaryState(TAG_Character_EnterCinematic);
}

void ASLPlayerCharacter::EndCinematic()
{
	if (USL25DMovementHandlerComponent* CombatHandler = FindComponentByClass<USL25DMovementHandlerComponent>())
	{
		CombatHandler->SetComponentTickEnabled(true);
	}

	SetPrimaryState(TAG_Character_Movement_Idle);
}

void ASLPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (Sword)
	{
		Sword->Destroy();
		Sword = nullptr;
	}

	if (Shield)
	{
		Shield->Destroy();
		Shield = nullptr;
	}
}

void ASLPlayerCharacter::OnEmpoweredStateChanged(const bool bIsEmpowered)
{
	if (ASLDefaultSword* PlayerSword = Cast<ASLDefaultSword>(Sword))
	{
		PlayerSword->UpdateMaterialByGauge(bIsEmpowered ? 100 : 0);
	}
}

void ASLPlayerCharacter::DisableWeapons()
{
	if (Sword)
	{
		Sword->Destroy();
		Sword = nullptr;
	}

	if (Shield)
	{
		Shield->Destroy();
		Shield = nullptr;
	}
}

void ASLPlayerCharacter::ChangeVisibilityWeapons(bool bIsVisible)
{
	const bool bShouldBeHidden = !bIsVisible;

	if (Sword)
	{
		Sword->SetActorHiddenInGame(bShouldBeHidden);
	}

	if (Shield)
	{
		Shield->SetActorHiddenInGame(bShouldBeHidden);
	}
}

void ASLPlayerCharacter::ResetState()
{
	if (CashedPlayerState)
	{
		CashedPlayerState->ResetState();
		SetPrimaryState(TAG_Character_Movement_Idle);
		CachedMontageComponent->StopAllMontages(0.2);
		
		if (UMovementHandlerComponent* MoveComp = FindComponentByClass<UMovementHandlerComponent>())
		{
			MoveComp->ToggleCameraZoom(true);
		}
	}
}

void ASLPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASLPlayerCharacter::EnableLockOnMode()
{
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	AddSecondaryState(TAG_Character_LockOn);
}

void ASLPlayerCharacter::DisableLockOnMode()
{
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	RemoveSecondaryState(TAG_Character_LockOn);
	RemoveSecondaryState(TAG_Character_PrepareLockOn);
}

void ASLPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (UMovementHandlerComponent* MoveComp = FindComponentByClass<UMovementHandlerComponent>())
	{
		MoveComp->OnLanded(Hit);
	}

	LastLandTime = GetWorld()->GetTimeSeconds();
	UE_LOG(LogTemp, Log, TEXT("Landed at time: %f"), LastLandTime);
}

void ASLPlayerCharacter::OnPlayerHpChanged(float MaxHp, const float CurrentHp)
{
	if (CurrentHp <= 0 && !bIsAlreadyDied)
	{
		CachedMontageComponent->PlayHitMontage("Death");
		SetPrimaryState(TAG_Character_Dead);
		bIsAlreadyDied = true;

		if (UMovementHandlerComponent* MoveComp = FindComponentByClass<UMovementHandlerComponent>())
		{
			MoveComp->DisableLock();
			MoveComp->ToggleCameraZoom(false, 1000.0f);
		}
	}
}

void ASLPlayerCharacter::AttachItemToHand(AActor* ItemActor, const FName SocketName) const
{
	if (!ItemActor || !GetMesh()) return;

	ItemActor->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		SocketName
	);
}

void ASLPlayerCharacter::BeginBlast(const EItemType ItemType)
{
	switch (ItemType)
	{
	case EItemType::IT_Sword:
		if (SwordClass)
		{
			if (Sword)
			{
				Sword->SetActorHiddenInGame(true);
			}

			StartOrbitWithClone(SwordClass, EItemType::IT_Sword, EOrbitDirection::CounterClockwise);
			StartOrbitWithClone(SwordClass, EItemType::IT_Sword, EOrbitDirection::CounterClockwise);
		}
		break;
	case EItemType::IT_Shield:
		if (ShieldClass)
		{
			if (Shield)
			{
				Shield->SetActorHiddenInGame(true);
			}

			StartOrbitWithClone(ShieldClass, EItemType::IT_Shield, EOrbitDirection::Clockwise);
			StartOrbitWithClone(ShieldClass, EItemType::IT_Shield, EOrbitDirection::Clockwise);
		}
		break;
	}
}

void ASLPlayerCharacter::StartOrbitWithClone(const TSubclassOf<AActor>& ObjectClass, const EItemType ItemType, const EOrbitDirection OrbitDirection)
{
	if (!ObjectClass) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	FRotator SpawnRot = GetActorRotation();
	//SpawnRot.Pitch += 90.f;

	AActor* OrbitObject = nullptr;
	switch (ItemType)
	{
	case EItemType::IT_Sword:
		OrbitObject = GetWorld()->SpawnActor<AActor>(ObjectClass, GetActorLocation(), SpawnRot, SpawnParams);
		break;
	case EItemType::IT_Shield:
		OrbitObject = GetWorld()->SpawnActor<AActor>(ObjectClass, GetActorLocation(), SpawnRot, SpawnParams);
		break;
	}

	if (ASLItem* OrbitItem = Cast<ASLItem>(OrbitObject))
	{
		OrbitItem->OnItemOverlap.AddDynamic(this, &ASLPlayerCharacter::OnItemSweeped);
		OrbitItem->StartOrbit(this, ItemType, OrbitDirection);
	}
}

bool ASLPlayerCharacter::IsBlocking() const
{
	return PrimaryStateTags.HasTag(TAG_Character_Defense_Block);
}

// 상태 관리
void ASLPlayerCharacter::SetPrimaryState(FGameplayTag NewState)
{
	if (!bIsBlockChangeable) return;
	PrimaryStateTags.Reset();
	PrimaryStateTags.AddTag(NewState);
}

void ASLPlayerCharacter::AddSecondaryState(FGameplayTag NewState)
{
	SecondaryStateTags.AddTag(NewState);
}

void ASLPlayerCharacter::RemoveSecondaryState(FGameplayTag StateToRemove)
{
	SecondaryStateTags.RemoveTag(StateToRemove);
}

bool ASLPlayerCharacter::IsInPrimaryState(FGameplayTag StateToCheck) const
{
	return PrimaryStateTags.HasTag(StateToCheck);
}

bool ASLPlayerCharacter::HasSecondaryState(FGameplayTag StateToCheck) const
{
	return SecondaryStateTags.HasTag(StateToCheck);
}

void ASLPlayerCharacter::RemovePrimaryState(FGameplayTag StateToRemove)
{
	PrimaryStateTags.RemoveTag(StateToRemove);
}

void ASLPlayerCharacter::ClearAllStateTags()
{
	if (!PrimaryStateTags.IsEmpty())
	{
		PrimaryStateTags.Reset();
	}

	if (!SecondaryStateTags.IsEmpty())
	{
		SecondaryStateTags.Reset();
	}
}

void ASLPlayerCharacter::ClearStateTags(const TArray<FGameplayTag>& PrimaryExceptTagList,
                                        const TArray<FGameplayTag>& SecondaryExceptTagList)
{
	FGameplayTagContainer PrimaryExceptTags;
	for (const FGameplayTag& Tag : PrimaryExceptTagList)
	{
		if (PrimaryStateTags.HasTag(Tag))
		{
			PrimaryExceptTags.AddTag(Tag);
		}
	}

	FGameplayTagContainer SecondaryExceptTags;
	for (const FGameplayTag& Tag : SecondaryExceptTagList)
	{
		if (SecondaryStateTags.HasTag(Tag))
		{
			SecondaryExceptTags.AddTag(Tag);
		}
	}

	if (!PrimaryStateTags.IsEmpty())
	{
		PrimaryStateTags.Reset();
		PrimaryStateTags = PrimaryExceptTags;
	}

	if (!SecondaryStateTags.IsEmpty())
	{
		SecondaryStateTags.Reset();
		SecondaryStateTags = SecondaryExceptTags;
	}
}

void ASLPlayerCharacter::ToggleBlock(bool bDoUnlock)
{
	bIsBlockChangeable = bDoUnlock;
}

bool ASLPlayerCharacter::IsConditionBlocked(EQueryType QueryType) const
{
	if (const FTagQueryAssetPair* QueryPair = ConditionQueryMap.Find(QueryType))
	{
		// Primary + Secondary를 합친 태그 컨테이너 생성
		FGameplayTagContainer CombinedTags = PrimaryStateTags;
		CombinedTags.AppendTags(SecondaryStateTags);

		// 쿼리 에셋 리스트 검사
		for (const TObjectPtr<UTagQueryDataAsset>& QueryAsset : QueryPair->QueryAssets)
		{
			if (QueryAsset && QueryAsset->TagQuery.Matches(CombinedTags))
			{
				return true;
			}
		}
	}
	return false;
}

void ASLPlayerCharacter::PrintPrimaryStateTags() const
{
	for (const FGameplayTag& Tag : PrimaryStateTags.GetGameplayTagArray())
	{
		UE_LOG(LogTemp, Warning, TEXT("[PrimaryState] %s"), *Tag.ToString());
	}
}

void ASLPlayerCharacter::OnItemSweeped(AActor* OverlappedActor, FHitResult Hit)
{
	//UE_LOG(LogTemp, Warning, TEXT("OverlappedActor [%s]"), *OverlappedActor->GetName());
	if (UBattleComponent* BattleComp = this->FindComponentByClass<UBattleComponent>())
	{
		BattleComp->SendHitResult(OverlappedActor, Hit, EAttackAnimType::AAT_Skill2);
	}
}

#include "SLPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GamePlayTag/GamePlayTag.h"
#include "MovementHandlerComponent/SLMovementHandlerComponent.h"
#include "RadarComponent/CollisionRadarComponent.h"

ASLPlayerCharacter::ASLPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

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
		Sword = GetWorld()->SpawnActor<AActor>(SwordClass, GetActorLocation(), GetActorRotation());
		AttachItemToHand(Sword, TEXT("r_weapon_socket"));
		Sword->SetOwner(this);
	}

	if (ShieldClass)
	{
		Shield = GetWorld()->SpawnActor<AActor>(ShieldClass, GetActorLocation(), GetActorRotation());
		AttachItemToHand(Shield, TEXT("l_weapon_socket"));
		Shield->SetOwner(this);
	}
	
	SetPrimaryState(TAG_Character_Movement_Idle);

	PrintPrimaryStateTags();
}

void ASLPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetMovementComponent()->IsFalling())
	{
		// 점프 중: AI와 충돌 무시
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}
	else
	{
		// 착지 후: AI와 충돌 활성화
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
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

void ASLPlayerCharacter::AttachItemToHand(AActor* ItemActor, const FName SocketName) const
{
	if (!ItemActor || !GetMesh()) return;

	ItemActor->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		SocketName
	);
}

bool ASLPlayerCharacter::IsBlocking() const
{
	return PrimaryStateTags.HasTag(TAG_Character_Defense_Block);
}

// 상태 관리
void ASLPlayerCharacter::SetPrimaryState(FGameplayTag NewState)
{
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

void ASLPlayerCharacter::ClearStateTags(const TArray<FGameplayTag>& PrimaryExceptTagList, const TArray<FGameplayTag>& SecondaryExceptTagList)
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

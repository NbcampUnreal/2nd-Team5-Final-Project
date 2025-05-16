#include "SLPlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "GamePlayTag/GamePlayTag.h"
#include "MovementHandlerComponent/SLMovementHandlerComponent.h"

ASLPlayerCharacter::ASLPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	//GetCharacterMovement()->bOrientRotationToMovement = true; // Zelda-like
	//GetCharacterMovement()->RotationRate = FRotator(0.f, 80.f, 0.f);
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true; // 자체 회전 제어
	//CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 3.f;

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
	}

	if (ShieldClass)
	{
		Shield = GetWorld()->SpawnActor<AActor>(ShieldClass, GetActorLocation(), GetActorRotation());
		AttachItemToHand(Shield, TEXT("l_weapon_socket"));
	}

	SetPrimaryState(TAG_Character_Movement_Dash);

	if (PrimaryStateTags.HasTag(TAG_Character_Movement_Run))
	{
		UE_LOG(LogTemp, Warning, TEXT("Run"));
	}

	PrintPrimaryStateTags();
}

void ASLPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASLPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
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

void ASLPlayerCharacter::ClearAllPrimaryStates()
{
	PrimaryStateTags.Reset();
}

void ASLPlayerCharacter::ClearAllSecondaryStates()
{
	SecondaryStateTags.Reset();
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

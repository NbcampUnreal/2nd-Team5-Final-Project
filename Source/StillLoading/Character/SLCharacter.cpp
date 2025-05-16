#include "SLCharacter.h"

#include "GamePlayTag/GamePlayTag.h"
#include "MovementHandlerComponent/SLMovementHandlerComponent.h"

ASLCharacter::ASLCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASLCharacter::BeginPlay()
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

void ASLCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASLCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASLCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (UMovementHandlerComponent* MoveComp = FindComponentByClass<UMovementHandlerComponent>())
	{
		MoveComp->OnLanded(Hit);
	}

	LastLandTime = GetWorld()->GetTimeSeconds();
	UE_LOG(LogTemp, Log, TEXT("Landed at time: %f"), LastLandTime);
}

void ASLCharacter::AttachItemToHand(AActor* ItemActor, const FName SocketName) const
{
	if (!ItemActor || !GetMesh()) return;

	ItemActor->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		SocketName
	);
	
	UAnimMontage* b = Hi();
}

bool ASLCharacter::IsBlocking() const
{
	return PrimaryStateTags.HasTag(TAG_Character_Defense_Block);
}

// 상태 관리
void ASLCharacter::SetPrimaryState(FGameplayTag NewState)
{
	PrimaryStateTags.AddTag(NewState);
}

void ASLCharacter::AddSecondaryState(FGameplayTag NewState)
{
	SecondaryStateTags.AddTag(NewState);
}

void ASLCharacter::RemoveSecondaryState(FGameplayTag StateToRemove)
{
	SecondaryStateTags.RemoveTag(StateToRemove);
}

bool ASLCharacter::IsInPrimaryState(FGameplayTag StateToCheck) const
{
	return PrimaryStateTags.HasTag(StateToCheck);
}

bool ASLCharacter::HasSecondaryState(FGameplayTag StateToCheck) const
{
	return SecondaryStateTags.HasTag(StateToCheck);
}

void ASLCharacter::RemovePrimaryState(FGameplayTag StateToRemove)
{
	PrimaryStateTags.RemoveTag(StateToRemove);
}

void ASLCharacter::ClearAllPrimaryStates()
{
	PrimaryStateTags.Reset();
}

void ASLCharacter::ClearAllSecondaryStates()
{
	SecondaryStateTags.Reset();
}

bool ASLCharacter::IsConditionBlocked(EQueryType QueryType) const
{
	if (const FTagQueryAssetPair* QueryPair = ConditionQueryMap.Find(QueryType))
	{
		for (const TObjectPtr<UTagQueryDataAsset>& QueryAsset : QueryPair->QueryAssets)
		{
			if (QueryAsset && QueryAsset->TagQuery.Matches(PrimaryStateTags))
			{
				return true;
			}
		}
	}
	return false;
}

void ASLCharacter::PrintPrimaryStateTags() const
{
	for (const FGameplayTag& Tag : PrimaryStateTags.GetGameplayTagArray())
	{
		UE_LOG(LogTemp, Warning, TEXT("[PrimaryState] %s"), *Tag.ToString());
	}
}

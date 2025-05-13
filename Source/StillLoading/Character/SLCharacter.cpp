#include "SLCharacter.h"

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
}

/*/ 이동 시작 → 태그 추가
    if (bIsMoving && !bWasMoving)
    {
        CharacterTags.AddTag(TAG_Character_Movement_Run);
        UE_LOG(LogTemp, Log, TEXT("Run tag added (GAS-less)"));
    }

    // 이동 중지 → 태그 제거
    if (!bIsMoving && bWasMoving)
    {
        CharacterTags.RemoveTag(TAG_Character_Movement_Run);
        UE_LOG(LogTemp, Log, TEXT("Run tag removed (GAS-less)"));
    }

    bWasMoving = bIsMoving;

    // 예시: 상태 체크
    if (CharacterTags.HasTag(TAG_Character_Movement_Run))
    {
        UE_LOG(LogTemp, Log, TEXT("Character is running (GAS-less)"));
    }

 */
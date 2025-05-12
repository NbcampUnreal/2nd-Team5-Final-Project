#include "BattleComponent.h"

#include "Components/CapsuleComponent.h"

UBattleComponent::UBattleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	AttackCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("AttackCollision"));
	AttackCollision->InitCapsuleSize(10.f, 50.f);
	if (GetOwner() && GetOwner()->GetRootComponent())
	{
		AttackCollision->SetupAttachment(GetOwner()->GetRootComponent());
	}
	AttackCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	AttackCollision->SetGenerateOverlapEvents(true);

	SetIsReplicatedByDefault(true); // Component Replication 설정
}

void UBattleComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AttackCollision)
	{
		AttackCollision->OnComponentBeginOverlap.AddDynamic(this, &UBattleComponent::OnAttackOverlap);
	}
}

void UBattleComponent::ReceiveBattleDamage_Implementation(const float DamageAmount)
{
	UE_LOG(LogTemp, Warning, TEXT("BattleComponent: Received %f Damage"), DamageAmount);

	// TODO: HP 감소, 이펙트, 애니메이션 처리 등
}

void UBattleComponent::ServerAttack_Implementation(AActor* Target)
{
	Attack(Target);
}

bool UBattleComponent::ServerAttack_Validate(AActor* Target)
{
	return true;
}

void UBattleComponent::Attack(AActor* Target)
{
	if (Target && Target->GetClass()->ImplementsInterface(USLBattleInterface::StaticClass()))
	{
		ISLBattleInterface::Execute_ReceiveBattleDamage(Target, 10.0f);
	}
}

void UBattleComponent::OnAttackOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetOwner()) return;
	
	if (OtherActor->GetRootComponent()->GetCollisionObjectType() == EnemyChannel)
	{
		if (GetOwner()->HasAuthority())
			Attack(OtherActor);
		else
			ServerAttack(OtherActor);
	}
}


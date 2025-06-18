
#include "SLDestructibleActor.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Character/SLPlayerCharacterBase.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"

ASLDestructibleActor::ASLDestructibleActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// BattleComponent 생성
	BattleComponent = CreateDefaultSubobject<UBattleComponent>(TEXT("BattleComponent"));

	CurrentState = EDestructibleState::Inactive;
	CurrentHealth = 0.0f;
	ActorIndex = -1;
	MaxHealth = 10.0f;
	DestroyEffect = nullptr;
	DestroySound = nullptr;
	ActivationSound = nullptr;
}

void ASLDestructibleActor::BeginPlay()
{
	Super::BeginPlay();

	// BattleComponent 델리게이트 바인딩
	if (BattleComponent)
	{
		BattleComponent->OnCharacterHited.AddDynamic(this, &ASLDestructibleActor::OnBattleComponentHit);
	}

	// 충돌 설정
	SetupCollisionResponse();

	// 초기 비주얼 업데이트
	UpdateActorVisuals();
}

void ASLDestructibleActor::ActivateActor()
{
	if (CurrentState == EDestructibleState::Destroyed)
	{
		return;
	}

	SetActorState(EDestructibleState::Active);
	CurrentHealth = MaxHealth;

	if (ActivationSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ActivationSound,
			GetActorLocation()
		);
	}

	UE_LOG(LogTemp, Log, TEXT("Destructible Actor %d activated"), ActorIndex);
}

void ASLDestructibleActor::DeactivateActor()
{
	if (CurrentState == EDestructibleState::Destroyed)
	{
		return;
	}

	SetActorState(EDestructibleState::Inactive);
	CurrentHealth = 0.0f;

	UE_LOG(LogTemp, Log, TEXT("Destructible Actor %d deactivated"), ActorIndex);
}

void ASLDestructibleActor::DestroyActor()
{
	if (CurrentState == EDestructibleState::Destroyed)
	{
		return;
	}

	SetActorState(EDestructibleState::Destroyed);
	CurrentHealth = 0.0f;

	PlayDestroyEffects();
	OnActorDestroyed();

	if (OnDestructibleDestroyed.IsBound())
	{
		OnDestructibleDestroyed.Broadcast(ActorIndex);
	}

	UE_LOG(LogTemp, Log, TEXT("Destructible Actor %d destroyed"), ActorIndex);
}

bool ASLDestructibleActor::CanBeDestroyed() const
{
	return CurrentState == EDestructibleState::Active && CurrentHealth > 0.0f;
}

void ASLDestructibleActor::SetActorIndex(int32 Index)
{
	ActorIndex = Index;
}

int32 ASLDestructibleActor::GetActorIndex() const
{
	return ActorIndex;
}

EDestructibleState ASLDestructibleActor::GetCurrentState() const
{
	return CurrentState;
}

float ASLDestructibleActor::GetCurrentHealth() const
{
	return CurrentHealth;
}

float ASLDestructibleActor::GetMaxHealth() const
{
	return MaxHealth;
}

void ASLDestructibleActor::TakeDamage(float DamageAmount)
{
	if (!CanBeDestroyed())
	{
		return;
	}

	CurrentHealth -= DamageAmount;

	if (CurrentHealth <= 0.0f)
	{
		DestroyActor();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Destructible Actor %d took damage: %f, Health remaining: %f"), 
			ActorIndex, DamageAmount, CurrentHealth);
	}
}

void ASLDestructibleActor::OnBattleComponentHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType HitAnimType)
{
	if (!CanBeDestroyed())
	{
		return;
	}

	// 플레이어의 BattleComponent를 통한 공격 처리
	if (ASLPlayerCharacterBase* PlayerCharacter = Cast<ASLPlayerCharacterBase>(DamageCauser))
	{
		CurrentHealth -= DamageAmount;

		if (CurrentHealth <= 0.0f)
		{
			DestroyActor();
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Destructible Actor %d damaged by BattleComponent. Damage: %f, Health: %f"), 
				ActorIndex, DamageAmount, CurrentHealth);
		}
	}
}

void ASLDestructibleActor::SetupCollisionResponse()
{
	// 하위 클래스에서 구체적인 충돌 컴포넌트에 대해 설정
	// 기본적으로 모든 PrimitiveComponent에 ECC_GameTraceChannel1 설정
	TArray<UPrimitiveComponent*> PrimitiveComponents;
	GetComponents<UPrimitiveComponent>(PrimitiveComponents);

	for (UPrimitiveComponent* PrimComp : PrimitiveComponents)
	{
		if (PrimComp)
		{
			PrimComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
		}
	}
}

void ASLDestructibleActor::UpdateActorVisuals()
{
	// 기본 구현 - 하위 클래스에서 오버라이드
}

void ASLDestructibleActor::PlayDestroyEffects()
{
	if (!GetWorld())
	{
		return;
	}

	if (DestroyEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			DestroyEffect,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector(1.0f),
			true,
			true,
			ENCPoolMethod::None,
			true
		);
	}

	if (DestroySound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			DestroySound,
			GetActorLocation()
		);
	}
}

void ASLDestructibleActor::SetActorState(EDestructibleState NewState)
{
	if (CurrentState != NewState)
	{
		CurrentState = NewState;
		UpdateActorVisuals();
	}
}

void ASLDestructibleActor::OnActorDestroyed()
{
	// 기본 구현 - 하위 클래스에서 오버라이드하여 추가 로직 구현
}
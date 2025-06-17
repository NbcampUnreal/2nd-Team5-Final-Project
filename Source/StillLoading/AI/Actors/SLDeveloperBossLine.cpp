#include "SLDeveloperBossLine.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Character/SLPlayerCharacterBase.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "CollisionQueryParams.h"

ASLDeveloperBossLine::ASLDeveloperBossLine()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	LineMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LineMeshComponent"));
	LineMeshComponent->SetupAttachment(RootComponent);

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(RootComponent);

	// BattleComponent 추가
	BattleComponent = CreateDefaultSubobject<UBattleComponent>(TEXT("BattleComponent"));

	CurrentState = EBossLineState::Inactive;
	CurrentHealth = 0.0f;
	LineIndex = -1;
	MaxHealth = 10.0f;
	LineMesh = nullptr;
	InactiveMaterial = nullptr;
	ActiveMaterial = nullptr;
	DestroyEffect = nullptr;
	DestroySound = nullptr;
	ActivationSound = nullptr;
}

void ASLDeveloperBossLine::BeginPlay()
{
	Super::BeginPlay();

	if (CollisionComponent)
	{
		CollisionComponent->OnComponentHit.AddDynamic(this, &ASLDeveloperBossLine::OnLineHit);
		// BattleComponent 스윕에 반응하도록 충돌 설정
		CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	}

	// BattleComponent 델리게이트 바인딩
	if (BattleComponent)
	{
		BattleComponent->OnCharacterHited.AddDynamic(this, &ASLDeveloperBossLine::OnBattleComponentHit);
	}

	if (LineMesh && LineMeshComponent)
	{
		LineMeshComponent->SetStaticMesh(LineMesh);
	}

	UpdateLineVisuals();
}

void ASLDeveloperBossLine::ActivateLine()
{
	if (CurrentState == EBossLineState::Destroyed)
	{
		return;
	}

	SetLineState(EBossLineState::Active);
	CurrentHealth = MaxHealth;

	if (ActivationSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ActivationSound,
			GetActorLocation()
		);
	}

	UE_LOG(LogTemp, Log, TEXT("Boss Line %d activated"), LineIndex);
}

void ASLDeveloperBossLine::DeactivateLine()
{
	if (CurrentState == EBossLineState::Destroyed)
	{
		return;
	}

	SetLineState(EBossLineState::Inactive);
	CurrentHealth = 0.0f;

	UE_LOG(LogTemp, Log, TEXT("Boss Line %d deactivated"), LineIndex);
}

void ASLDeveloperBossLine::DestroyLine()
{
	if (CurrentState == EBossLineState::Destroyed)
	{
		return;
	}

	SetLineState(EBossLineState::Destroyed);
	CurrentHealth = 0.0f;

	PlayDestroyEffects();

	if (OnBossLineDestroyed.IsBound())
	{
		OnBossLineDestroyed.Broadcast(LineIndex);
	}

	UE_LOG(LogTemp, Log, TEXT("Boss Line %d destroyed"), LineIndex);
}

bool ASLDeveloperBossLine::CanBeDestroyed() const
{
	return CurrentState == EBossLineState::Active && CurrentHealth > 0.0f;
}

void ASLDeveloperBossLine::SetLineIndex(int32 Index)
{
	LineIndex = Index;
}

int32 ASLDeveloperBossLine::GetLineIndex() const
{
	return LineIndex;
}

EBossLineState ASLDeveloperBossLine::GetCurrentState() const
{
	return CurrentState;
}

void ASLDeveloperBossLine::OnLineHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || !CanBeDestroyed())
	{
		return;
	}

	// 플레이어의 직접적인 물리 충돌 처리 (기존 방식 유지)
	if (ASLPlayerCharacterBase* PlayerCharacter = Cast<ASLPlayerCharacterBase>(OtherActor))
	{
		float Damage = 1.0f;
		CurrentHealth -= Damage;

		if (CurrentHealth <= 0.0f)
		{
			DestroyLine();
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Boss Line %d damaged by collision. Health: %f"), LineIndex, CurrentHealth);
		}
	}
}

void ASLDeveloperBossLine::OnBattleComponentHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType HitAnimType)
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
			DestroyLine();
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Boss Line %d damaged by BattleComponent. Damage: %f, Health: %f"), 
				LineIndex, DamageAmount, CurrentHealth);
		}
	}
}

void ASLDeveloperBossLine::UpdateLineVisuals()
{
	if (!LineMeshComponent)
	{
		return;
	}

	switch (CurrentState)
	{
	case EBossLineState::Inactive:
		LineMeshComponent->SetVisibility(false);
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (InactiveMaterial)
		{
			LineMeshComponent->SetMaterial(0, InactiveMaterial);
		}
		break;

	case EBossLineState::Active:
		LineMeshComponent->SetVisibility(true);
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		if (ActiveMaterial)
		{
			LineMeshComponent->SetMaterial(0, ActiveMaterial);
		}
		break;

	case EBossLineState::Destroyed:
		LineMeshComponent->SetVisibility(false);
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

void ASLDeveloperBossLine::PlayDestroyEffects()
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

void ASLDeveloperBossLine::SetLineState(EBossLineState NewState)
{
	if (CurrentState != NewState)
	{
		CurrentState = NewState;
		UpdateLineVisuals();
	}
}
#include "SLDeveloperRoomCable.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Character/SLPlayerCharacterBase.h"
#include "Engine/Engine.h"

ASLDeveloperRoomCable::ASLDeveloperRoomCable()
{
	// 기본 StaticMeshComp를 숨기고 새로운 컴포넌트들 생성
	if (StaticMeshComp)
	{
		StaticMeshComp->SetVisibility(false);
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	LineMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LineMeshComponent"));
	LineMeshComponent->SetupAttachment(RootComponent);

	LineCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("LineCollisionComponent"));
	LineCollisionComponent->SetupAttachment(RootComponent);

	// 초기값 설정
	CurrentLineState = EBossLineState::Inactive;
	LineIndex = -1;
	MaxHp = 10;
	CurrentHp = 10;
	
	// TriggerType을 Hit으로 설정하여 공격으로만 파괴 가능하게 함
	TriggerType = ESLReactiveTriggerType::ERT_Hit;

	LineMesh = nullptr;
	InactiveMaterial = nullptr;
	ActiveMaterial = nullptr;
}

void ASLDeveloperRoomCable::BeginPlay()
{
	Super::BeginPlay();

	if (LineCollisionComponent)
	{
		LineCollisionComponent->OnComponentHit.AddDynamic(this, &ASLDeveloperRoomCable::OnLineHit);
	}

	if (LineMesh && LineMeshComponent)
	{
		LineMeshComponent->SetStaticMesh(LineMesh);
	}

	SetupLineCollisionResponse();
	UpdateLineVisuals();
}

void ASLDeveloperRoomCable::ActivateLine()
{
	SetLineState(EBossLineState::Active);
	CurrentHp = MaxHp;

	UE_LOG(LogTemp, Log, TEXT("Boss Line %d activated"), LineIndex);
}

void ASLDeveloperRoomCable::DeactivateLine()
{
	if (CurrentLineState == EBossLineState::Destroyed)
	{
		return;
	}

	SetLineState(EBossLineState::Inactive);
	CurrentHp = 0;

	UE_LOG(LogTemp, Log, TEXT("Boss Line %d deactivated"), LineIndex);
}

void ASLDeveloperRoomCable::DestroyLine()
{
	if (CurrentLineState == EBossLineState::Destroyed)
	{
		return;
	}

	SetLineState(EBossLineState::Destroyed);
	CurrentHp = 0;

	// 파괴 이펙트는 부모 클래스에서 처리
	if (DestroyEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			DestroyEffect,
			GetActorLocation(),
			GetActorRotation(),
			FVector(1.0f),
			true,
			true,
			ENCPoolMethod::None,
			true
		);
	}

	if (OnBossLineDestroyed.IsBound())
	{
		OnBossLineDestroyed.Broadcast(LineIndex);
	}

	// 부모 클래스의 OnObjectBreaked도 호출
	OnObjectBreaked.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("Boss Line %d destroyed"), LineIndex);
}

void ASLDeveloperRoomCable::SetLineIndex(int32 Index)
{
	LineIndex = Index;
}

int32 ASLDeveloperRoomCable::GetLineIndex() const
{
	return LineIndex;
}

EBossLineState ASLDeveloperRoomCable::GetCurrentLineState() const
{
	return CurrentLineState;
}

bool ASLDeveloperRoomCable::CanBeDestroyed() const
{
	return CurrentLineState == EBossLineState::Active && CurrentHp > 0;
}

void ASLDeveloperRoomCable::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	if (!CanBeDestroyed())
	{
		return;
	}

	// 부모 클래스의 체력 시스템 사용
	if (CurrentHp > 0)
	{
		CurrentHp--;
		
		if (CurrentHp <= 0)
		{
			DestroyLine();
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Boss Line %d damaged. Health remaining: %d"), LineIndex, CurrentHp);
		}
	}
}

void ASLDeveloperRoomCable::OnLineHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || !CanBeDestroyed())
	{
		return;
	}

	// 플레이어의 직접적인 물리 충돌 처리
	if (ASLPlayerCharacterBase* PlayerCharacter = Cast<ASLPlayerCharacterBase>(OtherActor))
	{
		TriggerReact(PlayerCharacter, ESLReactiveTriggerType::ERT_Hit);
		UE_LOG(LogTemp, Log, TEXT("Boss Line %d hit by collision"), LineIndex);
	}
}

void ASLDeveloperRoomCable::UpdateLineVisuals()
{
	if (!LineMeshComponent)
	{
		return;
	}

	switch (CurrentLineState)
	{
	case EBossLineState::Inactive:
		LineMeshComponent->SetVisibility(false);
		LineCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (InactiveMaterial)
		{
			LineMeshComponent->SetMaterial(0, InactiveMaterial);
		}
		break;

	case EBossLineState::Active:
		LineMeshComponent->SetVisibility(true);
		LineCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		if (ActiveMaterial)
		{
			LineMeshComponent->SetMaterial(0, ActiveMaterial);
		}
		break;

	case EBossLineState::Destroyed:
		LineMeshComponent->SetVisibility(false);
		LineCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

void ASLDeveloperRoomCable::SetLineState(EBossLineState NewState)
{
	if (CurrentLineState != NewState)
	{
		CurrentLineState = NewState;
		UpdateLineVisuals();
	}
}

void ASLDeveloperRoomCable::SetupLineCollisionResponse()
{
	if (LineCollisionComponent)
	{
		LineCollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	}

	if (LineMeshComponent)
	{
		LineMeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	}
}
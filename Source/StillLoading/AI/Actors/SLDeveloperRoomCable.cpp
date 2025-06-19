#include "SLDeveloperRoomCable.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Character/SLPlayerCharacterBase.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

ASLDeveloperRoomCable::ASLDeveloperRoomCable()
{
	PrimaryActorTick.bCanEverTick = true;

	LineCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("LineCollisionComponent"));
	LineCollisionComponent->SetupAttachment(RootComponent);

	LineSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LineSkeletalMeshComponent"));
	LineSkeletalMeshComponent->SetupAttachment(RootComponent);

	// 초기값 설정
	CurrentLineState = EBossLineState::Inactive;
	LineIndex = -1;
	MaxHp = 10;
	CurrentHp = 10;
	
	// TriggerType을 Hit으로 설정하여 공격으로만 파괴 가능하게 함
	TriggerType = ESLReactiveTriggerType::ERT_Hit;

	CableSkeletalMesh = nullptr;
	InactiveMaterial = nullptr;
	ActiveMaterial = nullptr;
	DamagedMaterial = nullptr;
	HitEffect = nullptr;
	HitSound = nullptr;
	MaterialFlashDuration = 0.2f;
	ShakeIntensity = 10.0f;
	ShakeDuration = 0.3f;
	ShakeFrequency = 20.0f;

	// Shake variables
	OriginalLocation = FVector::ZeroVector;
	ShakeTimeElapsed = 0.0f;
	bIsShaking = false;
}

void ASLDeveloperRoomCable::BeginPlay()
{
	Super::BeginPlay();

	if (LineCollisionComponent)
	{
		LineCollisionComponent->OnComponentHit.AddDynamic(this, &ASLDeveloperRoomCable::OnLineHit);
	}

	// 스켈레탈 메시 설정
	if (CableSkeletalMesh && LineSkeletalMeshComponent)
	{
		LineSkeletalMeshComponent->SetSkeletalMesh(CableSkeletalMesh);
	}

	// StaticMeshComp가 있다면 숨기기 (부모에서 상속받은 것)
	if (StaticMeshComp)
	{
		StaticMeshComp->SetVisibility(false);
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 원래 위치 저장
	OriginalLocation = GetActorLocation();

	SetupLineCollisionResponse();
	UpdateLineVisuals();
}

void ASLDeveloperRoomCable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 흔들기 효과 업데이트
	if (bIsShaking)
	{
		UpdateShakeEffect(DeltaTime);
	}
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

	// 블루프린트에서 구현할 이벤트 호출 (이펙트만 처리)
	OnLineDestroyed();

	UE_LOG(LogTemp, Log, TEXT("Boss Line %d destroyed - effects handled by blueprint"), LineIndex);
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

void ASLDeveloperRoomCable::ForceDestroy()
{
	// 블루프린트에서 직접 파괴를 호출할 수 있는 함수
	DestroyLine();
}

void ASLDeveloperRoomCable::HideAndDisable()
{
	// 블루프린트에서 이펙트 처리 후 호출할 함수
	if (LineSkeletalMeshComponent)
	{
		LineSkeletalMeshComponent->SetVisibility(false);
		LineSkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (LineCollisionComponent)
	{
		LineCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 액터 자체를 숨기고 비활성화
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	// 이펙트 처리가 완료된 후 델리게이트 브로드캐스트
	if (OnBossLineDestroyed.IsBound())
	{
		OnBossLineDestroyed.Broadcast(LineIndex);
	}

	// 부모 클래스의 OnObjectBreaked도 호출
	OnObjectBreaked.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("Boss Line %d hidden and disabled - delegates broadcasted"), LineIndex);
}

void ASLDeveloperRoomCable::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	if (!CanBeDestroyed())
	{
		return;
	}

	// 히트 피드백 재생
	PlayHitFeedback(GetActorLocation());

	// 흔들기 효과 시작
	StartShakeEffect();

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
			// 데미지를 받았을 때 머티리얼 플래시
			FlashDamagedMaterial();
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
		// 히트 피드백을 충돌 지점에서 재생
		PlayHitFeedback(Hit.ImpactPoint);
		
		// 흔들기 효과 시작
		StartShakeEffect();
		
		TriggerReact(PlayerCharacter, ESLReactiveTriggerType::ERT_Hit);
		UE_LOG(LogTemp, Log, TEXT("Boss Line %d hit by collision"), LineIndex);
	}
}

void ASLDeveloperRoomCable::UpdateLineVisuals()
{
	if (!LineSkeletalMeshComponent)
	{
		return;
	}

	switch (CurrentLineState)
	{
	case EBossLineState::Inactive:
		LineSkeletalMeshComponent->SetVisibility(false);
		LineCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (InactiveMaterial)
		{
			LineSkeletalMeshComponent->SetMaterial(0, InactiveMaterial);
		}
		break;

	case EBossLineState::Active:
		LineSkeletalMeshComponent->SetVisibility(true);
		LineCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		if (ActiveMaterial)
		{
			LineSkeletalMeshComponent->SetMaterial(0, ActiveMaterial);
		}
		break;

	case EBossLineState::Destroyed:
		LineSkeletalMeshComponent->SetVisibility(false);
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

	if (LineSkeletalMeshComponent)
	{
		LineSkeletalMeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	}
}

void ASLDeveloperRoomCable::PlayHitFeedback(const FVector& HitLocation)
{
	// 히트 이펙트 재생
	if (HitEffect && GetWorld())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			HitEffect,
			HitLocation,
			FRotator::ZeroRotator,
			FVector(1.0f),
			true,
			true,
			ENCPoolMethod::None,
			true
		);
	}

	// 히트 사운드 재생
	if (HitSound && GetWorld())
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			HitSound,
			HitLocation
		);
	}

	UE_LOG(LogTemp, Log, TEXT("Boss Line %d hit feedback played at %s"), LineIndex, *HitLocation.ToString());
}

void ASLDeveloperRoomCable::FlashDamagedMaterial()
{
	if (!LineSkeletalMeshComponent || !DamagedMaterial)
	{
		return;
	}

	// 기존 타이머가 있다면 클리어
	if (GetWorld() && MaterialFlashTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(MaterialFlashTimerHandle);
	}

	// 데미지 머티리얼로 변경
	LineSkeletalMeshComponent->SetMaterial(0, DamagedMaterial);

	// 일정 시간 후 원래 머티리얼로 복구
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			MaterialFlashTimerHandle,
			[this]()
			{
				if (LineSkeletalMeshComponent && CurrentLineState == EBossLineState::Active && ActiveMaterial)
				{
					LineSkeletalMeshComponent->SetMaterial(0, ActiveMaterial);
				}
			},
			MaterialFlashDuration,
			false
		);
	}

	UE_LOG(LogTemp, Log, TEXT("Boss Line %d material flashed"), LineIndex);
}

void ASLDeveloperRoomCable::StartShakeEffect()
{
	if (bIsShaking)
	{
		return; // 이미 흔들리고 있으면 무시
	}

	bIsShaking = true;
	ShakeTimeElapsed = 0.0f;
	OriginalLocation = GetActorLocation();

	// 흔들기 타이머 설정
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			ShakeTimerHandle,
			[this]()
			{
				bIsShaking = false;
				// 원래 위치로 복구
				SetActorLocation(OriginalLocation);
				UE_LOG(LogTemp, Log, TEXT("Boss Line %d shake effect ended"), LineIndex);
			},
			ShakeDuration,
			false
		);
	}

	UE_LOG(LogTemp, Log, TEXT("Boss Line %d shake effect started"), LineIndex);
}

void ASLDeveloperRoomCable::UpdateShakeEffect(float DeltaTime)
{
	if (!bIsShaking)
	{
		return;
	}

	ShakeTimeElapsed += DeltaTime;

	// 사인파를 이용한 랜덤 흔들림 계산
	float ShakeX = FMath::Sin(ShakeTimeElapsed * ShakeFrequency) * ShakeIntensity * FMath::FRandRange(-1.0f, 1.0f);
	float ShakeY = FMath::Cos(ShakeTimeElapsed * ShakeFrequency * 1.3f) * ShakeIntensity * FMath::FRandRange(-1.0f, 1.0f);
	float ShakeZ = FMath::Sin(ShakeTimeElapsed * ShakeFrequency * 0.7f) * ShakeIntensity * 0.5f * FMath::FRandRange(-1.0f, 1.0f);

	// 시간이 지날수록 흔들림 감소
	float DecayFactor = FMath::Max(0.0f, 1.0f - (ShakeTimeElapsed / ShakeDuration));
	ShakeX *= DecayFactor;
	ShakeY *= DecayFactor;
	ShakeZ *= DecayFactor;

	// 새로운 위치 설정
	FVector NewLocation = OriginalLocation + FVector(ShakeX, ShakeY, ShakeZ);
	SetActorLocation(NewLocation);
}
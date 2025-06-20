#include "SLDestructibleActor.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "Character/SLPlayerCharacterBase.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

ASLDestructibleActor::ASLDestructibleActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// TriggerType을 Hit으로 설정하여 공격으로만 파괴 가능하게 함
	TriggerType = ESLReactiveTriggerType::ERT_Hit;

	// 기본 체력 설정
	MaxHp = 3;
	CurrentHp = 3;

	// Material 초기화
	NormalMaterial = nullptr;
	DamagedMaterial = nullptr;
	MaterialFlashDuration = 0.2f;

	// Effect 초기화
	HitEffect = nullptr;
	HitSound = nullptr;

	// Crack 초기화
	CrackDecalMaterial = nullptr;
	CrackDecalSize = FVector(50.0f, 50.0f, 50.0f);
	CrackDecalLifeTime = 10.0f;

	// Shake 초기화
	ShakeIntensity = 10.0f;
	ShakeDuration = 0.3f;
	ShakeFrequency = 20.0f;
	OriginalLocation = FVector::ZeroVector;
	ShakeTimeElapsed = 0.0f;
	bIsShaking = false;
}

void ASLDestructibleActor::BeginPlay()
{
	Super::BeginPlay();

	// 원래 위치 저장
	OriginalLocation = GetActorLocation();

	// 일반 머티리얼 설정
	if (NormalMaterial && StaticMeshComp)
	{
		StaticMeshComp->SetMaterial(0, NormalMaterial);
	}

	// 콜리전 설정
	SetupCollisionSettings();
}

void ASLDestructibleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 흔들기 효과 업데이트
	if (bIsShaking)
	{
		UpdateShakeEffect(DeltaTime);
	}
}

void ASLDestructibleActor::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	if (!CanTakeDamage())
	{
		return;
	}

	FVector HitLocation = GetActorLocation();
	FVector HitNormal = FVector::UpVector;

	// 캐릭터가 있다면 더 정확한 히트 위치 계산
	if (InCharacter)
	{
		FVector CharacterLocation = InCharacter->GetActorLocation();
		FVector Direction = (GetActorLocation() - CharacterLocation).GetSafeNormal();
		HitLocation = GetActorLocation() + Direction * -50.0f; // 표면 근처로 조정
		HitNormal = Direction;
	}

	// 균열 데칼 추가
	AddCrackDecal(HitLocation, HitNormal);

	// 히트 피드백 재생
	PlayHitFeedback(HitLocation);

	// 흔들기 효과 시작
	StartShakeEffect();

	// 데미지 처리
	TakeDamage();

	UE_LOG(LogTemp, Log, TEXT("Destructible Actor hit. Health remaining: %d"), CurrentHp);
}

void ASLDestructibleActor::TakeDamage()
{
	if (CurrentHp > 0)
	{
		CurrentHp--;
		
		if (CurrentHp <= 0)
		{
			DestroyActor();
		}
		else
		{
			// 데미지를 받았을 때 머티리얼 플래시
			FlashDamagedMaterial();
		}
	}
}

bool ASLDestructibleActor::CanTakeDamage() const
{
	return CurrentHp > 0;
}

void ASLDestructibleActor::AddCrackDecal(const FVector& HitLocation, const FVector& HitNormal)
{
	if (!CrackDecalMaterial || !GetWorld())
	{
		return;
	}

	// 데칼 컴포넌트 생성
	UDecalComponent* CrackDecal = NewObject<UDecalComponent>(this);
	if (!CrackDecal)
	{
		return;
	}

	// 데칼 설정
	CrackDecal->SetDecalMaterial(CrackDecalMaterial);
	CrackDecal->DecalSize = CrackDecalSize;
	CrackDecal->SetWorldLocation(HitLocation);
	
	// 노멀 방향으로 데칼 회전 설정
	FRotator DecalRotation = FRotationMatrix::MakeFromZ(HitNormal).Rotator();
	// 랜덤 Z축 회전 추가로 균열 방향 다양화
	DecalRotation.Roll += FMath::FRandRange(-180.0f, 180.0f);
	CrackDecal->SetWorldRotation(DecalRotation);

	// 데칼을 루트 컴포넌트에 부착
	CrackDecal->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	CrackDecal->RegisterComponent();

	// 배열에 추가
	CrackDecals.Add(CrackDecal);

	// 수명 설정 (선택사항)
	if (CrackDecalLifeTime > 0.0f)
	{
		FTimerHandle DecalTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			DecalTimerHandle,
			[this, CrackDecal]()
			{
				if (IsValid(CrackDecal))
				{
					CrackDecals.Remove(CrackDecal);
					CrackDecal->DestroyComponent();
				}
			},
			CrackDecalLifeTime,
			false
		);
	}

	UE_LOG(LogTemp, Log, TEXT("Crack decal added at %s"), *HitLocation.ToString());
}

void ASLDestructibleActor::ClearAllCracks()
{
	for (UDecalComponent* CrackDecal : CrackDecals)
	{
		if (IsValid(CrackDecal))
		{
			CrackDecal->DestroyComponent();
		}
	}
	CrackDecals.Empty();

	UE_LOG(LogTemp, Log, TEXT("All crack decals cleared"));
}

void ASLDestructibleActor::PlayHitFeedback(const FVector& HitLocation)
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

	UE_LOG(LogTemp, Log, TEXT("Destructible Actor hit feedback played at %s"), *HitLocation.ToString());
}

void ASLDestructibleActor::FlashDamagedMaterial()
{
	if (!StaticMeshComp || !DamagedMaterial)
	{
		return;
	}

	// 기존 타이머가 있다면 클리어
	if (GetWorld() && MaterialFlashTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(MaterialFlashTimerHandle);
	}

	// 데미지 머티리얼로 변경
	StaticMeshComp->SetMaterial(0, DamagedMaterial);

	// 일정 시간 후 원래 머티리얼로 복구
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			MaterialFlashTimerHandle,
			[this]()
			{
				if (StaticMeshComp && NormalMaterial && CurrentHp > 0)
				{
					StaticMeshComp->SetMaterial(0, NormalMaterial);
				}
			},
			MaterialFlashDuration,
			false
		);
	}

	UE_LOG(LogTemp, Log, TEXT("Destructible Actor material flashed"));
}

void ASLDestructibleActor::StartShakeEffect()
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
				UE_LOG(LogTemp, Log, TEXT("Destructible Actor shake effect ended"));
			},
			ShakeDuration,
			false
		);
	}

	UE_LOG(LogTemp, Log, TEXT("Destructible Actor shake effect started"));
}

void ASLDestructibleActor::UpdateShakeEffect(float DeltaTime)
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

void ASLDestructibleActor::DestroyActor()
{
	// 콜리전 비활성화
	if (StaticMeshComp)
	{
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 모든 균열 데칼 제거
	ClearAllCracks();

	// 부모 클래스의 OnInteracted를 호출하여 기존 파괴 로직 실행
	Super::OnInteracted(nullptr, ESLReactiveTriggerType::ERT_Hit);

	UE_LOG(LogTemp, Log, TEXT("Destructible Actor destroyed"));
}

void ASLDestructibleActor::SetupCollisionSettings()
{
	if (StaticMeshComp)
	{
		// 콜리전 활성화
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        
		// 오브젝트 타입을 WorldStatic으로 설정 (환경 오브젝트)
		StaticMeshComp->SetCollisionObjectType(ECC_WorldStatic);
        
		// 모든 채널에 대해 Block 응답
		StaticMeshComp->SetCollisionResponseToAllChannels(ECR_Block);
        
		// 카메라는 무시
		StaticMeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
        
		// 플레이어(Pawn)에 대해서는 확실히 Block
		StaticMeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
}
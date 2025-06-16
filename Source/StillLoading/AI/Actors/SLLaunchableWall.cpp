// Copyright Epic Games, Inc. All Rights Reserved.

#include "SLLaunchableWall.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Character/SLPlayerCharacterBase.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Character/DataAsset/AttackDataAsset.h"

ASLLaunchableWall::ASLLaunchableWall()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	RotationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RotationTimeline"));
	YMovementTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("YMovementTimeline"));
	SpacingTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("SpacingTimeline"));

	// 기본값 설정
	NumberOfWallParts = 5;
	WallPartSpacing = 100.0f;
	LaunchDelay = 0.2f;
	LaunchSpeed = 1000.0f;
	WallPartScale = FVector(1.0f, 1.0f, 1.0f);
	bArrangeHorizontally = true;
	ProjectileGravityScale = 0.0f;
	
	// 회전 설정
	bApplyRotationBeforeLaunch = true;
	RotationDelay = 0.1f;
	RotationDuration = 0.5f;
	RotationCurve = nullptr;
	
	// 스폰 회전 설정
	SpawnRotation = FRotator::ZeroRotator;
	bApplySpawnRotationOnConstruction = true;
	
	// Pre-Launch Animation 설정
	bEnablePreLaunchAnimation = true;
	YMovementDistance = 100.0f;
	SpacingExpansionDistance = 50.0f;
	YMovementDuration = 0.5f;
	SpacingAnimationDuration = 0.5f;
	YMovementCurve = nullptr;
	SpacingAnimationCurve = nullptr;
	
	// 이펙트 설정
	CharacterHitEffect = nullptr;
	CharacterHitSound = nullptr;
	GroundHitEffect = nullptr;
	GroundHitSound = nullptr;
	
	// 메시 설정
	bUseCustomMeshPerPart = false;
	DefaultWallMesh = nullptr;
	
	// 내부 변수 초기화
	CurrentLaunchIndex = 0;
	LaunchedPartsCount = 0;
	bIsLaunching = false;
	bIsYMovementAnimating = false;
	bIsSpacingAnimating = false;
	
	// Timeline 회전 변수 초기화
	CurrentRotatingPartIndex = -1;
	StartRotation = FRotator::ZeroRotator;
	TargetRotation = FRotator::ZeroRotator;

	// 벽 파트들을 생성자에서 생성
	CreateWallParts();
}

void ASLLaunchableWall::BeginPlay()
{
	Super::BeginPlay();
	
	if (RotationTimeline)
	{
		RotationTimelineUpdateDelegate.BindUFunction(this, FName("OnRotationTimelineUpdate"));
		RotationTimelineFinishedDelegate.BindUFunction(this, FName("OnRotationTimelineFinished"));
		
		if (RotationCurve)
		{
			RotationTimeline->AddInterpFloat(RotationCurve, RotationTimelineUpdateDelegate);
		}
		RotationTimeline->SetTimelineFinishedFunc(RotationTimelineFinishedDelegate);
		RotationTimeline->SetTimelineLength(RotationDuration);
		RotationTimeline->SetLooping(false);
	}
	
	// YMovementTimeline 설정
	if (YMovementTimeline)
	{
		YMovementTimelineUpdateDelegate.BindUFunction(this, FName("OnYMovementTimelineUpdate"));
		YMovementTimelineFinishedDelegate.BindUFunction(this, FName("OnYMovementTimelineFinished"));
		
		if (YMovementCurve)
		{
			YMovementTimeline->AddInterpFloat(YMovementCurve, YMovementTimelineUpdateDelegate);
		}
		YMovementTimeline->SetTimelineFinishedFunc(YMovementTimelineFinishedDelegate);
		YMovementTimeline->SetTimelineLength(YMovementDuration);
		YMovementTimeline->SetLooping(false);
	}
	
	// SpacingTimeline 설정
	if (SpacingTimeline)
	{
		SpacingTimelineUpdateDelegate.BindUFunction(this, FName("OnSpacingTimelineUpdate"));
		SpacingTimelineFinishedDelegate.BindUFunction(this, FName("OnSpacingTimelineFinished"));
		
		if (SpacingAnimationCurve)
		{
			SpacingTimeline->AddInterpFloat(SpacingAnimationCurve, SpacingTimelineUpdateDelegate);
		}
		SpacingTimeline->SetTimelineFinishedFunc(SpacingTimelineFinishedDelegate);
		SpacingTimeline->SetTimelineLength(SpacingAnimationDuration);
		SpacingTimeline->SetLooping(false);
	}
	
	SetupWallParts();
	
	// 원본 위치 저장
	OriginalWallPartPositions.Empty();
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	for (int32 i = 0; i < ActiveParts && i < WallParts.Num(); i++)
	{
		if (WallParts[i])
		{
			OriginalWallPartPositions.Add(WallParts[i]->GetRelativeLocation());
		}
	}
	
	// BeginPlay에서 스폰 회전 적용
	if (bApplySpawnRotationOnConstruction)
	{
		ApplySpawnRotation();
	}
}

void ASLLaunchableWall::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	// 에디터에서 실시간 미리보기 업데이트
	UpdateWallPartsVisibility();
	RefreshWallLayout();
	RefreshWallMeshes();
	
	// Timeline 길이 업데이트
	if (RotationTimeline)
	{
		RotationTimeline->SetTimelineLength(RotationDuration);
	}
	
	if (YMovementTimeline)
	{
		YMovementTimeline->SetTimelineLength(YMovementDuration);
	}
	
	if (SpacingTimeline)
	{
		SpacingTimeline->SetTimelineLength(SpacingAnimationDuration);
	}
	
	// Construction에서 스폰 회전 적용 (에디터 미리보기용)
	if (bApplySpawnRotationOnConstruction)
	{
		ApplySpawnRotation();
	}
}

void ASLLaunchableWall::LaunchWallToPlayer()
{
	if (!CanLaunch())
	{
		return;
	}
	
	bIsLaunching = true;
	CurrentLaunchIndex = 0;
	
	// Pre-Launch 애니메이션이 활성화되어 있으면 애니메이션부터 시작
	if (bEnablePreLaunchAnimation)
	{
		StartPreLaunchAnimation();
	}
	else
	{
		// 애니메이션 없이 바로 발사
		LaunchNextPart();
	}
}

bool ASLLaunchableWall::CanLaunch() const
{
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	return !bIsLaunching && !bIsYMovementAnimating && !bIsSpacingAnimating && WallParts.Num() > 0 && LaunchedPartsCount < ActiveParts;
}

void ASLLaunchableWall::StartPreLaunchAnimation()
{
	// Y방향 이동부터 시작
	StartYMovementAnimation();
}

void ASLLaunchableWall::StartYMovementAnimation()
{
	if (!YMovementTimeline)
	{
		// Timeline이 없으면 바로 간격 확장으로
		StartSpacingAnimation();
		return;
	}
	
	bIsYMovementAnimating = true;
	
	// 원본 위치 저장
	OriginalWallPartPositions.Empty();
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	for (int32 i = 0; i < ActiveParts && i < WallParts.Num(); i++)
	{
		if (WallParts[i])
		{
			OriginalWallPartPositions.Add(WallParts[i]->GetRelativeLocation());
		}
	}
	
	YMovementTimeline->PlayFromStart();
}

void ASLLaunchableWall::OnYMovementTimelineUpdate(float Value)
{
	UpdateWallPartsYPosition(Value);
}

void ASLLaunchableWall::OnYMovementTimelineFinished()
{
	bIsYMovementAnimating = false;
	
	// Y 이동 완료 후 Y 이동된 위치를 저장
	YMovedWallPartPositions.Empty();
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	for (int32 i = 0; i < ActiveParts && i < WallParts.Num(); i++)
	{
		if (WallParts[i])
		{
			YMovedWallPartPositions.Add(WallParts[i]->GetRelativeLocation());
		}
	}
	
	// 간격 확장 애니메이션 시작
	StartSpacingAnimation();
}

void ASLLaunchableWall::StartSpacingAnimation()
{
	if (!SpacingTimeline)
	{
		// Timeline이 없으면 바로 발사 시작
		LaunchNextPart();
		return;
	}
	
	bIsSpacingAnimating = true;
	SpacingTimeline->PlayFromStart();
}

void ASLLaunchableWall::OnSpacingTimelineUpdate(float Value)
{
	UpdateWallPartsSpacing(Value);
}

void ASLLaunchableWall::OnSpacingTimelineFinished()
{
	bIsSpacingAnimating = false;
	
	// 간격 확장 완료 후 실제 발사 시작
	LaunchNextPart();
}

void ASLLaunchableWall::UpdateWallPartsYPosition(float AnimationProgress)
{
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	
	for (int32 i = 0; i < ActiveParts && i < WallParts.Num() && i < OriginalWallPartPositions.Num(); i++)
	{
		if (WallParts[i])
		{
			FVector OriginalPos = OriginalWallPartPositions[i];
			
			// Y 방향 이동만
			float YOffset = YMovementDistance * AnimationProgress;
			
			FVector NewPosition = OriginalPos;
			NewPosition.Y += YOffset;
			WallParts[i]->SetRelativeLocation(NewPosition);
		}
	}
}

void ASLLaunchableWall::UpdateWallPartsSpacing(float AnimationProgress)
{
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	
	for (int32 i = 0; i < ActiveParts && i < WallParts.Num() && i < YMovedWallPartPositions.Num(); i++)
	{
		if (WallParts[i])
		{
			FVector YMovedPos = YMovedWallPartPositions[i];
			
			// 간격 확장만
			float SpacingOffset = 0.0f;
			if (bArrangeHorizontally)
			{
				// 가로 배치일 때 X축 간격 확장
				SpacingOffset = (i - (ActiveParts - 1) * 0.5f) * SpacingExpansionDistance * AnimationProgress;
				
				FVector NewPosition = YMovedPos;
				NewPosition.X += SpacingOffset;
				WallParts[i]->SetRelativeLocation(NewPosition);
			}
			else
			{
				// 세로 배치일 때 Z축 간격 확장
				SpacingOffset = (i - (ActiveParts - 1) * 0.5f) * SpacingExpansionDistance * AnimationProgress;
				
				FVector NewPosition = YMovedPos;
				NewPosition.Z += SpacingOffset;
				WallParts[i]->SetRelativeLocation(NewPosition);
			}
		}
	}
}

void ASLLaunchableWall::RefreshWallMeshes()
{
	// 활성화된 벽 파트들의 메시를 현재 설정에 맞게 업데이트
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	
	for (int32 i = 0; i < ActiveParts && i < WallParts.Num(); i++)
	{
		if (WallParts[i])
		{
			UStaticMesh* MeshToUse = GetMeshForPart(i);
			if (MeshToUse)
			{
				WallParts[i]->SetStaticMesh(MeshToUse);
			}
		}
	}
}

void ASLLaunchableWall::RefreshWallLayout()
{
	// 활성화된 벽 파트들의 위치를 현재 설정에 맞게 업데이트
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	
	for (int32 i = 0; i < ActiveParts; i++)
	{
		if (WallParts.IsValidIndex(i) && WallParts[i])
		{
			FVector PartLocation;
			if (bArrangeHorizontally)
			{
				// 가로 배치 (X축)
				PartLocation = FVector(i * WallPartSpacing, 0.0f, 0.0f);
			}
			else
			{
				// 세로 배치 (Z축)
				PartLocation = FVector(0.0f, 0.0f, i * WallPartSpacing);
			}
			
			WallParts[i]->SetRelativeLocation(PartLocation);
			WallParts[i]->SetRelativeScale3D(WallPartScale);
		}
	}
	
	// 레이아웃 변경 후 스폰 회전 다시 적용
	if (bApplySpawnRotationOnConstruction)
	{
		ApplySpawnRotation();
	}
}

void ASLLaunchableWall::ApplySpawnRotation()
{
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	
	for (int32 i = 0; i < ActiveParts && i < WallParts.Num(); i++)
	{
		if (WallParts[i])
		{
			ApplySpawnRotationToWallPart(WallParts[i], i);
		}
	}
}

void ASLLaunchableWall::SetSpawnRotation(const FRotator& NewRotation)
{
	SpawnRotation = NewRotation;
	
	// 즉시 회전 적용
	ApplySpawnRotation();
}

void ASLLaunchableWall::ApplySpawnRotationToWallPart(UStaticMeshComponent* WallPart, int32 PartIndex)
{
	if (!WallPart)
	{
		return;
	}
	
	WallPart->SetRelativeRotation(SpawnRotation);
}

void ASLLaunchableWall::OnWallPartHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor)
	{
		return;
	}

	// 히트된 벽 파트의 인덱스 찾기
	int32 WallPartIndex = -1;
	for (int32 i = 0; i < WallParts.Num(); i++)
	{
		if (WallParts[i] == HitComponent)
		{
			WallPartIndex = i;
			break;
		}
	}

	if (WallPartIndex == -1 || !WallPartStates.IsValidIndex(WallPartIndex))
	{
		return;
	}

	// 발사된 상태가 아니면 데미지를 주지 않음
	if (WallPartStates[WallPartIndex] != EWallPartState::Launched)
	{
		return;
	}

	// 플레이어(캐릭터)에 충돌한 경우
	if (ASLPlayerCharacterBase* PlayerCharacter = Cast<ASLPlayerCharacterBase>(OtherActor))
	{
		ApplyDamageToPlayer(OtherActor, Hit, WallPartIndex);
		PlayCharacterHitEffects(Hit.ImpactPoint, OtherActor);
		DestroyWallPart(WallPartIndex);
		return;
	}

	// 월드 오브젝트(바닥, 벽 등)에 충돌한 경우
	if (OtherComponent && 
		(OtherComponent->GetCollisionObjectType() == ECC_WorldStatic || 
		 OtherComponent->GetCollisionObjectType() == ECC_WorldDynamic))
	{
		PlayGroundHitEffects(Hit.ImpactPoint);
		DestroyWallPart(WallPartIndex);
		return;
	}
}

void ASLLaunchableWall::SetupWallParts()
{
	// 상태 배열 초기화
	WallPartStates.SetNum(MaxWallParts);
	for (int32 i = 0; i < MaxWallParts; i++)
	{
		WallPartStates[i] = EWallPartState::Inactive;
	}

	// 활성화된 벽 파트들만 설정
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	
	for (int32 i = 0; i < ActiveParts && i < WallParts.Num(); i++)
	{
		if (WallParts[i])
		{
			// 메시 설정
			UStaticMesh* MeshToUse = GetMeshForPart(i);
			if (MeshToUse)
			{
				WallParts[i]->SetStaticMesh(MeshToUse);
			}
			
			// 충돌 이벤트 바인딩 (중복 바인딩 방지)
			if (!WallParts[i]->OnComponentHit.IsBound())
			{
				WallParts[i]->OnComponentHit.AddDynamic(this, &ASLLaunchableWall::OnWallPartHit);
			}

			// 초기 상태를 비활성으로 설정
			WallPartStates[i] = EWallPartState::Inactive;
		}
	}
	
	// 가시성과 레이아웃 새로고침
	UpdateWallPartsVisibility();
	RefreshWallLayout();
}

void ASLLaunchableWall::LaunchNextPart()
{
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	
	if (CurrentLaunchIndex >= ActiveParts)
	{
		return;
	}

	// 회전 적용 여부에 따라 분기
	if (bApplyRotationBeforeLaunch)
	{
		// Timeline 기반 부드러운 회전 시작
		RotateCurrentPart();
	}
	else
	{
		// 회전 없이 바로 발사
		LaunchCurrentPart();
	}
}

void ASLLaunchableWall::CheckAllPartsLaunched()
{
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	
	if (LaunchedPartsCount >= ActiveParts)
	{
		bIsLaunching = false;
		
		// 모든 파트 발사 완료 이벤트 발생
		if (OnAllWallPartsLaunched.IsBound())
		{
			OnAllWallPartsLaunched.Broadcast();
		}
	}
}

void ASLLaunchableWall::RotateCurrentPart()
{
	if (CurrentLaunchIndex >= WallParts.Num())
	{
		return;
	}

	UStaticMeshComponent* CurrentPart = WallParts[CurrentLaunchIndex];
	
	if (CurrentPart)
	{
		// 상태를 회전중으로 변경
		if (WallPartStates.IsValidIndex(CurrentLaunchIndex))
		{
			WallPartStates[CurrentLaunchIndex] = EWallPartState::Rotating;
		}

		CurrentPart->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		
		StartSmoothRotation();
	}
}

void ASLLaunchableWall::LaunchCurrentPart()
{
	if (CurrentLaunchIndex >= WallParts.Num())
	{
		return;
	}

	UStaticMeshComponent* CurrentPart = WallParts[CurrentLaunchIndex];
	UProjectileMovementComponent* CurrentMovement = ProjectileMovements[CurrentLaunchIndex];

	if (CurrentPart && CurrentMovement)
	{
		if (WallPartStates.IsValidIndex(CurrentLaunchIndex))
		{
			WallPartStates[CurrentLaunchIndex] = EWallPartState::Launched;
		}

		if (!bApplyRotationBeforeLaunch)
		{
			CurrentPart->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			
			SetWallPartTopFaceToPlayer(CurrentPart);
		}
		
		FVector LaunchDirection = GetPlayerDirection();
		CurrentMovement->Velocity = LaunchDirection * LaunchSpeed;
		CurrentMovement->SetActive(true);
		
		LaunchedPartsCount++;
		CurrentLaunchIndex++;

		int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
		if (CurrentLaunchIndex < ActiveParts)
		{
			GetWorldTimerManager().SetTimer(
				LaunchTimerHandle,
				this,
				&ASLLaunchableWall::LaunchNextPart,
				LaunchDelay,
				false
			);
		}
		else
		{
			CheckAllPartsLaunched();
		}
	}
}

void ASLLaunchableWall::StartSmoothRotation()
{
	if (CurrentLaunchIndex >= WallParts.Num() || !RotationTimeline)
	{
		return;
	}

	UStaticMeshComponent* CurrentPart = WallParts[CurrentLaunchIndex];
	if (!CurrentPart)
	{
		return;
	}

	// 회전 정보 저장
	CurrentRotatingPartIndex = CurrentLaunchIndex;
	StartRotation = CurrentPart->GetRelativeRotation();
	
	// 벽이 앞으로 넘어지도록 Pitch 90도 설정
	TargetRotation = StartRotation;
	TargetRotation.Roll = 90.0f;

	RotationTimeline->SetTimelineLength(RotationDuration);
	
	RotationTimeline->PlayFromStart();
}

void ASLLaunchableWall::OnRotationTimelineUpdate(float Value)
{
	if (CurrentRotatingPartIndex < 0 || CurrentRotatingPartIndex >= WallParts.Num() || !WallParts[CurrentRotatingPartIndex])
	{
		return;
	}

	UStaticMeshComponent* CurrentPart = WallParts[CurrentRotatingPartIndex];
	
	// 커브 값에 따른 회전 보간 (0.0~1.0)
	FRotator CurrentRotation = FMath::Lerp(StartRotation, TargetRotation, Value);
	CurrentPart->SetWorldRotation(CurrentRotation);
}

void ASLLaunchableWall::OnRotationTimelineFinished()
{
	// 최종 회전 적용 (정확성 보장)
	if (CurrentRotatingPartIndex >= 0 && CurrentRotatingPartIndex < WallParts.Num() && WallParts[CurrentRotatingPartIndex])
	{
		WallParts[CurrentRotatingPartIndex]->SetWorldRotation(TargetRotation);
	}

	// 초기화
	CurrentRotatingPartIndex = -1;

	// 회전 완료 후 잠시 딜레이를 두고 실제 발사
	GetWorldTimerManager().SetTimer(
		RotationTimerHandle,
		this,
		&ASLLaunchableWall::LaunchCurrentPart,
		RotationDelay,
		false
	);
}

void ASLLaunchableWall::ApplyDamageToPlayer(AActor* PlayerActor, const FHitResult& HitResult, int32 WallPartIndex)
{
	if (!PlayerActor)
	{
		return;
	}

	// BattleComponent를 통한 데미지 처리
	if (UBattleComponent* BattleComp = PlayerActor->FindComponentByClass<UBattleComponent>())
	{
		// 이 액터를 데미지 원인으로 설정하여 히트 결과 전송
		BattleComp->SendHitResult(PlayerActor, HitResult, EAttackAnimType::AAT_AIProjectile);
	}
}

void ASLLaunchableWall::PlayCharacterHitEffects(const FVector& HitLocation, AActor* HitActor)
{
	if (!GetWorld())
	{
		return;
	}

	// 캐릭터 히트 이펙트 재생
	if (CharacterHitEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			CharacterHitEffect,
			HitLocation,
			FRotator::ZeroRotator,
			FVector(1.0f)
		);
	}

	// 캐릭터 히트 사운드 재생
	if (CharacterHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			CharacterHitSound,
			HitLocation
		);
	}
}

void ASLLaunchableWall::PlayGroundHitEffects(const FVector& HitLocation)
{
	if (!GetWorld())
	{
		return;
	}

	// 바닥 히트 이펙트 재생
	if (GroundHitEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			GroundHitEffect,
			HitLocation,
			FRotator::ZeroRotator,
			FVector(1.0f),
			true,
			true,
			ENCPoolMethod::None,
			true
		);
	}

	// 바닥 히트 사운드 재생
	if (GroundHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			GroundHitSound,
			HitLocation
		);
	}
}

void ASLLaunchableWall::DestroyWallPart(int32 WallPartIndex)
{
	if (!WallParts.IsValidIndex(WallPartIndex) || !WallParts[WallPartIndex])
	{
		return;
	}

	// 벽 파트를 숨기고 비활성화
	WallParts[WallPartIndex]->SetVisibility(false);
	WallParts[WallPartIndex]->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 프로젝타일 무브먼트 비활성화
	if (ProjectileMovements.IsValidIndex(WallPartIndex) && ProjectileMovements[WallPartIndex])
	{
		ProjectileMovements[WallPartIndex]->SetActive(false);
	}

	// 상태를 비활성으로 변경
	if (WallPartStates.IsValidIndex(WallPartIndex))
	{
		WallPartStates[WallPartIndex] = EWallPartState::Inactive;
	}

	UE_LOG(LogTemp, Log, TEXT("Wall part %d destroyed"), WallPartIndex);
}

FVector ASLLaunchableWall::GetPlayerLocation() const
{
	if (!GetWorld())
	{
		return GetActorLocation();
	}

	// 첫 번째 플레이어 컨트롤러의 폰 가져오기
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController || !PlayerController->GetPawn())
	{
		return GetActorLocation();
	}

	return PlayerController->GetPawn()->GetActorLocation();
}

FVector ASLLaunchableWall::GetPlayerDirection() const
{
	if (!GetWorld())
	{
		return GetActorForwardVector();
	}

	// 첫 번째 플레이어 컨트롤러의 폰 가져오기
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController || !PlayerController->GetPawn())
	{
		return GetActorForwardVector();
	}

	// 플레이어 방향 계산
	FVector PlayerLocation = PlayerController->GetPawn()->GetActorLocation();
	FVector WallLocation = GetActorLocation();
	FVector DirectionToPlayer = (PlayerLocation - WallLocation).GetSafeNormal();

	return DirectionToPlayer;
}

void ASLLaunchableWall::SetWallPartTopFaceToPlayer(UStaticMeshComponent* WallPart) const
{
	if (!WallPart)
	{
		return;
	}

	FVector PlayerLocation = GetPlayerLocation();
	FVector WallPartLocation = WallPart->GetComponentLocation();
	
	// 플레이어 방향 벡터 계산
	FVector DirectionToPlayer = (PlayerLocation - WallPartLocation).GetSafeNormal();
	
	// 벽의 윗면(Z축)이 플레이어를 향하도록 회전 생성
	FRotator WallRotation = UKismetMathLibrary::MakeRotFromZ(DirectionToPlayer);
	
	WallPart->SetWorldRotation(WallRotation);
}

void ASLLaunchableWall::CreateWallParts()
{
	// 기존 배열 초기화
	WallParts.Empty();
	ProjectileMovements.Empty();
	WallPartStates.SetNum(MaxWallParts);

	// 최대 개수만큼 벽 파트들 생성
	for (int32 i = 0; i < MaxWallParts; i++)
	{
		CreateWallPart(i);
		WallPartStates[i] = EWallPartState::Inactive;
	}
}

void ASLLaunchableWall::CreateWallPart(int32 Index)
{
	// 스태틱 메시 컴포넌트 생성
	FString ComponentName = FString::Printf(TEXT("WallPart_%d"), Index);
	UStaticMeshComponent* WallPart = CreateDefaultSubobject<UStaticMeshComponent>(*ComponentName);
	
	if (WallPart)
	{
		WallPart->SetupAttachment(RootComponent);
		
		// 배치 방향에 따른 위치 설정
		FVector PartLocation;
		if (bArrangeHorizontally)
		{
			// 가로 배치 (X축)
			PartLocation = FVector(Index * WallPartSpacing, 0.0f, 0.0f);
		}
		else
		{
			// 세로 배치 (Z축)
			PartLocation = FVector(0.0f, 0.0f, Index * WallPartSpacing);
		}
		
		WallPart->SetRelativeLocation(PartLocation);
		
		// 크기 설정
		WallPart->SetRelativeScale3D(WallPartScale);
		
		WallParts.Add(WallPart);

		// 프로젝타일 무브먼트 컴포넌트 생성
		FString MovementComponentName = FString::Printf(TEXT("ProjectileMovement_%d"), Index);
		UProjectileMovementComponent* ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(*MovementComponentName);
		
		if (ProjectileMovement)
		{
			ProjectileMovement->SetUpdatedComponent(WallPart);
			ProjectileMovement->InitialSpeed = LaunchSpeed;
			ProjectileMovement->MaxSpeed = LaunchSpeed;
			ProjectileMovement->bRotationFollowsVelocity = false;
			ProjectileMovement->bShouldBounce = false;
			ProjectileMovement->ProjectileGravityScale = ProjectileGravityScale;
			ProjectileMovement->SetActive(false); 
			
			ProjectileMovements.Add(ProjectileMovement);
		}
	}
}

void ASLLaunchableWall::UpdateWallPartsVisibility()
{
	// NumberOfWallParts에 따라 필요한 개수만 보이게 설정
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	
	for (int32 i = 0; i < WallParts.Num(); i++)
	{
		if (WallParts[i])
		{
			bool bShouldBeVisible = (i < ActiveParts);
			WallParts[i]->SetVisibility(bShouldBeVisible);
			
			// 프로젝타일 컴포넌트도 함께 활성화/비활성화
			if (ProjectileMovements.IsValidIndex(i) && ProjectileMovements[i])
			{
				ProjectileMovements[i]->SetActive(false); // 항상 비활성화 상태로 시작
			}
		}
	}
}

UStaticMesh* ASLLaunchableWall::GetMeshForPart(int32 Index) const
{
	// 파트별 커스텀 메시 사용 옵션이 켜져있고, 해당 인덱스에 메시가 있으면 사용
	if (bUseCustomMeshPerPart && 
		WallPartMeshes.IsValidIndex(Index) && 
		WallPartMeshes[Index])
	{
		return WallPartMeshes[Index];
	}
	
	// 그렇지 않으면 기본 메시 사용
	return DefaultWallMesh;
}
// Copyright Epic Games, Inc. All Rights Reserved.

#include "SLLaunchableWall.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/SLAIBaseCharacter.h"
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
	PlayerAimTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("PlayerAimTimeline"));
	YMovementTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("YMovementTimeline"));
	SpacingTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("SpacingTimeline"));

	NumberOfWallParts = 5;
	WallPartSpacing = 100.0f;
	LaunchDelay = 0.2f;
	LaunchSpeed = 1000.0f;
	WallPartScale = FVector(1.0f, 1.0f, 1.0f);
	bArrangeHorizontally = true;
	ProjectileGravityScale = 0.0f;
	
	RotationDelay = 0.1f;
	RotationDuration = 0.5f;
	RotationCurve = nullptr;
	
	PlayerAimDuration = 0.3f;
	PlayerAimCurve = nullptr;
	
	SpawnRotation = FRotator::ZeroRotator;
	bApplySpawnRotationOnConstruction = true;
	
	bEnablePreLaunchAnimation = true;
	YMovementDistance = 100.0f;
	SpacingExpansionDistance = 50.0f;
	YMovementDuration = 0.5f;
	SpacingAnimationDuration = 0.5f;
	YMovementCurve = nullptr;
	SpacingAnimationCurve = nullptr;
	
	CharacterHitEffect = nullptr;
	CharacterHitSound = nullptr;
	GroundHitEffect = nullptr;
	GroundHitSound = nullptr;
	
	bUseCustomMeshPerPart = false;
	DefaultWallMesh = nullptr;
	
	CurrentLaunchIndex = 0;
	LaunchedPartsCount = 0;
	bIsLaunching = false;
	bIsYMovementAnimating = false;
	bIsSpacingAnimating = false;
	
	CurrentRotatingPartIndex = -1;
	StartRotation = FRotator::ZeroRotator;
	TargetRotation = FRotator::ZeroRotator;
	PlayerAimStartRotation = FRotator::ZeroRotator;
	PlayerAimTargetRotation = FRotator::ZeroRotator;

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
	
	if (PlayerAimTimeline)
	{
		PlayerAimTimelineUpdateDelegate.BindUFunction(this, FName("OnPlayerAimTimelineUpdate"));
		PlayerAimTimelineFinishedDelegate.BindUFunction(this, FName("OnPlayerAimTimelineFinished"));
		
		if (PlayerAimCurve)
		{
			PlayerAimTimeline->AddInterpFloat(PlayerAimCurve, PlayerAimTimelineUpdateDelegate);
		}
		PlayerAimTimeline->SetTimelineFinishedFunc(PlayerAimTimelineFinishedDelegate);
		PlayerAimTimeline->SetTimelineLength(PlayerAimDuration);
		PlayerAimTimeline->SetLooping(false);
	}
	
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
	
	OriginalWallPartPositions.Empty();
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	for (int32 i = 0; i < ActiveParts && i < WallParts.Num(); i++)
	{
		if (WallParts[i])
		{
			OriginalWallPartPositions.Add(WallParts[i]->GetRelativeLocation());
		}
	}
	
	if (bApplySpawnRotationOnConstruction)
	{
		ApplySpawnRotation();
	}
}

void ASLLaunchableWall::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	UpdateWallPartsVisibility();
	RefreshWallLayout();
	RefreshWallMeshes();
	
	if (RotationTimeline)
	{
		RotationTimeline->SetTimelineLength(RotationDuration);
	}
	
	if (PlayerAimTimeline)
	{
		PlayerAimTimeline->SetTimelineLength(PlayerAimDuration);
	}
	
	if (YMovementTimeline)
	{
		YMovementTimeline->SetTimelineLength(YMovementDuration);
	}
	
	if (SpacingTimeline)
	{
		SpacingTimeline->SetTimelineLength(SpacingAnimationDuration);
	}
	
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
	
	if (bEnablePreLaunchAnimation)
	{
		StartPreLaunchAnimation();
	}
	else
	{
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
	StartYMovementAnimation();
}

void ASLLaunchableWall::StartYMovementAnimation()
{
	if (!YMovementTimeline)
	{
		StartSpacingAnimation();
		return;
	}
	
	bIsYMovementAnimating = true;
	
	OriginalWallPartPositions.Empty();
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	for (int32 i = 0; i < ActiveParts && i < WallParts.Num(); i++)
	{
		if (WallParts[i])
		{
			OriginalWallPartPositions.Add(WallParts[i]->GetRelativeLocation());
		}
	}
	
	YMovementTimeline->SetTimelineLength(YMovementDuration);
	YMovementTimeline->PlayFromStart();
}

void ASLLaunchableWall::OnYMovementTimelineUpdate(float Value)
{
	UpdateWallPartsYPosition(Value);
}

void ASLLaunchableWall::OnYMovementTimelineFinished()
{
	bIsYMovementAnimating = false;
	
	YMovedWallPartPositions.Empty();
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	for (int32 i = 0; i < ActiveParts && i < WallParts.Num(); i++)
	{
		if (WallParts[i])
		{
			YMovedWallPartPositions.Add(WallParts[i]->GetRelativeLocation());
		}
	}
	
	StartSpacingAnimation();
}

void ASLLaunchableWall::StartSpacingAnimation()
{
	if (!SpacingTimeline)
	{
		LaunchNextPart();
		return;
	}
	
	bIsSpacingAnimating = true;
	SpacingTimeline->SetTimelineLength(SpacingAnimationDuration);
	SpacingTimeline->PlayFromStart();
}

void ASLLaunchableWall::OnSpacingTimelineUpdate(float Value)
{
	UpdateWallPartsSpacing(Value);
}

void ASLLaunchableWall::OnSpacingTimelineFinished()
{
	bIsSpacingAnimating = false;
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
			
			float SpacingOffset = 0.0f;
			if (bArrangeHorizontally)
			{
				SpacingOffset = (i - (ActiveParts - 1) * 0.5f) * SpacingExpansionDistance * AnimationProgress;
				
				FVector NewPosition = YMovedPos;
				NewPosition.X += SpacingOffset;
				WallParts[i]->SetRelativeLocation(NewPosition);
			}
			else
			{
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
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	
	for (int32 i = 0; i < ActiveParts; i++)
	{
		if (WallParts.IsValidIndex(i) && WallParts[i])
		{
			FVector PartLocation;
			if (bArrangeHorizontally)
			{
				PartLocation = FVector(i * WallPartSpacing, 0.0f, 0.0f);
			}
			else
			{
				PartLocation = FVector(0.0f, 0.0f, i * WallPartSpacing);
			}
			
			WallParts[i]->SetRelativeLocation(PartLocation);
			WallParts[i]->SetRelativeScale3D(WallPartScale);
		}
	}
	
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
	ApplySpawnRotation();
}

void ASLLaunchableWall::SetLaunchSpeed(float NewSpeed)
{
	LaunchSpeed = FMath::Max(0.0f, NewSpeed);
}

void ASLLaunchableWall::SetLaunchDelay(float NewDelay)
{
	LaunchDelay = FMath::Max(0.0f, NewDelay);
}

void ASLLaunchableWall::SetNumberOfWallParts(int32 NewCount)
{
	NumberOfWallParts = FMath::Clamp(NewCount, 1, MaxWallParts);
	UpdateWallPartsVisibility();
	RefreshWallLayout();
}

void ASLLaunchableWall::SetRotationDuration(float NewDuration)
{
	RotationDuration = FMath::Clamp(NewDuration, 0.1f, 2.0f);
}

void ASLLaunchableWall::SetPlayerAimDuration(float NewDuration)
{
	PlayerAimDuration = FMath::Max(0.1f, NewDuration);
}

void ASLLaunchableWall::SetAnimationDurations(float NewYMovementDuration, float NewSpacingDuration)
{
	YMovementDuration = FMath::Clamp(NewYMovementDuration, 0.1f, 3.0f);
	SpacingAnimationDuration = FMath::Clamp(NewSpacingDuration, 0.1f, 3.0f);
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

	// 죽은 AI 무시
	if (ASLAIBaseCharacter* AICharacter = Cast<ASLAIBaseCharacter>(OtherActor))
	{
		if (AICharacter->GetIsDead())
		{
			return;
		}
	}

	int32 WallPartIndex = -1;
	for (int32 i = 0; i < WallParts.Num(); i++)
	{
		if (WallParts[i] == HitComponent)
		{
			WallPartIndex = i;
			break;
		}
	}

	if (WallPartIndex == -1 || !WallPartStates.IsValidIndex(WallPartIndex) || WallPartStates[WallPartIndex] != EWallPartState::Launched)
	{
		return;
	}

	// 플레이어 충돌
	if (ASLPlayerCharacterBase* PlayerCharacter = Cast<ASLPlayerCharacterBase>(OtherActor))
	{
		ApplyDamageToPlayer(OtherActor, Hit, WallPartIndex);
		PlayCharacterHitEffects(Hit.ImpactPoint, OtherActor);
	}
	// 다른 모든 충돌
	else
	{
		PlayGroundHitEffects(Hit.ImpactPoint);
	}

	DestroyWallPart(WallPartIndex);
}

void ASLLaunchableWall::SetupWallParts()
{
	WallPartStates.SetNum(MaxWallParts);
	for (int32 i = 0; i < MaxWallParts; i++)
	{
		WallPartStates[i] = EWallPartState::Inactive;
	}

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
            
			// 충돌 설정
			SetupWallPartCollisions(WallParts[i]);
            
			if (!WallParts[i]->OnComponentHit.IsBound())
			{
				WallParts[i]->OnComponentHit.AddDynamic(this, &ASLLaunchableWall::OnWallPartHit);
			}

			WallPartStates[i] = EWallPartState::Inactive;
		}
	}
    
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

	RotateCurrentPart();
}

void ASLLaunchableWall::CheckAllPartsLaunched()
{
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	
	if (LaunchedPartsCount >= ActiveParts)
	{
		bIsLaunching = false;
		
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

		// ProjectileMovementComponent 속도 설정 업데이트
		CurrentMovement->InitialSpeed = LaunchSpeed;
		CurrentMovement->MaxSpeed = LaunchSpeed;
		
		// 윗면 방향의 반대로 발사 (윗면이 플레이어를 바라보고 있으므로 반대 방향으로 발사)
		FVector LaunchDirection = -CurrentPart->GetUpVector();
		
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

	CurrentRotatingPartIndex = CurrentLaunchIndex;
	StartRotation = CurrentPart->GetComponentRotation();
	
	// 윗면이 플레이어를 바라보도록 회전 계산
	FVector PartLocation = CurrentPart->GetComponentLocation();
	FVector DirectionToPlayer = GetPlayerDirectionFromPoint(PartLocation);
	TargetRotation = UKismetMathLibrary::MakeRotFromZ(-DirectionToPlayer);

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
	
	FRotator CurrentRotation = FMath::Lerp(StartRotation, TargetRotation, Value);
	CurrentPart->SetWorldRotation(CurrentRotation);
}

void ASLLaunchableWall::OnRotationTimelineFinished()
{
	CurrentRotatingPartIndex = -1;

	GetWorldTimerManager().SetTimer(
		RotationTimerHandle,
		this,
		&ASLLaunchableWall::LaunchCurrentPart,
		RotationDelay,
		false
	);
}

void ASLLaunchableWall::StartPlayerAimRotation()
{
	if (CurrentRotatingPartIndex < 0 || CurrentRotatingPartIndex >= WallParts.Num() || !WallParts[CurrentRotatingPartIndex])
	{
		return;
	}

	UStaticMeshComponent* CurrentPart = WallParts[CurrentRotatingPartIndex];
	if (!CurrentPart || !PlayerAimTimeline)
	{
		return;
	}

	if (WallPartStates.IsValidIndex(CurrentRotatingPartIndex))
	{
		WallPartStates[CurrentRotatingPartIndex] = EWallPartState::AimingToPlayer;
	}

	PlayerAimStartRotation = CurrentPart->GetComponentRotation();

	FVector PartLocation = CurrentPart->GetComponentLocation();
	FVector DirectionToPlayer = GetPlayerDirectionFromPoint(PartLocation);
	PlayerAimTargetRotation = UKismetMathLibrary::MakeRotFromZ(-DirectionToPlayer);
	PlayerAimTargetRotation.Roll = TargetRotation.Roll;

	PlayerAimTimeline->SetTimelineLength(PlayerAimDuration);
	PlayerAimTimeline->PlayFromStart();
}

void ASLLaunchableWall::OnPlayerAimTimelineUpdate(float Value)
{
	if (CurrentRotatingPartIndex < 0 || CurrentRotatingPartIndex >= WallParts.Num() || !WallParts[CurrentRotatingPartIndex])
	{
		return;
	}

	UStaticMeshComponent* CurrentPart = WallParts[CurrentRotatingPartIndex];
	
	FRotator CurrentRotation = FMath::Lerp(PlayerAimStartRotation, PlayerAimTargetRotation, Value);
	CurrentPart->SetWorldRotation(CurrentRotation);
}

void ASLLaunchableWall::OnPlayerAimTimelineFinished()
{
	CurrentRotatingPartIndex = -1;

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

	if (UBattleComponent* BattleComp = PlayerActor->FindComponentByClass<UBattleComponent>())
	{
		//BattleComp->SendHitResult(PlayerActor, HitResult, EAttackAnimType::AAT_AIProjectile);
		BattleComp->ReceiveHitResult(10.f,  this, HitResult,EAttackAnimType::AAT_AIProjectile);
	}
}

void ASLLaunchableWall::PlayCharacterHitEffects(const FVector& HitLocation, AActor* HitActor)
{
	if (!GetWorld())
	{
		return;
	}

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

	WallParts[WallPartIndex]->SetVisibility(false);
	WallParts[WallPartIndex]->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (ProjectileMovements.IsValidIndex(WallPartIndex) && ProjectileMovements[WallPartIndex])
	{
		ProjectileMovements[WallPartIndex]->SetActive(false);
	}

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

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController || !PlayerController->GetPawn())
	{
		return GetActorForwardVector();
	}

	FVector PlayerLocation = PlayerController->GetPawn()->GetActorLocation();
	FVector WallLocation = GetActorLocation();
	FVector DirectionToPlayer = (PlayerLocation - WallLocation).GetSafeNormal();

	return DirectionToPlayer;
}

FVector ASLLaunchableWall::GetPlayerDirectionFromPoint(const FVector& FromPoint) const
{
	if (!GetWorld())
	{
		return GetActorForwardVector();
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController || !PlayerController->GetPawn())
	{
		return GetActorForwardVector(); 
	}

	FVector PlayerLocation = PlayerController->GetPawn()->GetActorLocation();
	return (PlayerLocation - FromPoint).GetSafeNormal();
}

void ASLLaunchableWall::CreateWallParts()
{
	WallParts.Empty();
	ProjectileMovements.Empty();
	WallPartStates.SetNum(MaxWallParts);

	for (int32 i = 0; i < MaxWallParts; i++)
	{
		CreateWallPart(i);
		WallPartStates[i] = EWallPartState::Inactive;
	}
}

void ASLLaunchableWall::CreateWallPart(int32 Index)
{
	FString ComponentName = FString::Printf(TEXT("WallPart_%d"), Index);
	UStaticMeshComponent* WallPart = CreateDefaultSubobject<UStaticMeshComponent>(*ComponentName);
	
	if (WallPart)
	{
		WallPart->SetupAttachment(RootComponent);
		
		FVector PartLocation;
		if (bArrangeHorizontally)
		{
			PartLocation = FVector(Index * WallPartSpacing, 0.0f, 0.0f);
		}
		else
		{
			PartLocation = FVector(0.0f, 0.0f, Index * WallPartSpacing);
		}
		
		WallPart->SetRelativeLocation(PartLocation);
		WallPart->SetRelativeScale3D(WallPartScale);
		
		WallParts.Add(WallPart);

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
	int32 ActiveParts = FMath::Clamp(NumberOfWallParts, 1, MaxWallParts);
	
	for (int32 i = 0; i < WallParts.Num(); i++)
	{
		if (WallParts[i])
		{
			bool bShouldBeVisible = (i < ActiveParts);
			WallParts[i]->SetVisibility(bShouldBeVisible);
			
			if (ProjectileMovements.IsValidIndex(i) && ProjectileMovements[i])
			{
				ProjectileMovements[i]->SetActive(false);
			}
		}
	}
}

UStaticMesh* ASLLaunchableWall::GetMeshForPart(int32 Index) const
{
	if (bUseCustomMeshPerPart && 
		WallPartMeshes.IsValidIndex(Index) && 
		WallPartMeshes[Index])
	{
		return WallPartMeshes[Index];
	}
	
	return DefaultWallMesh;
}

void ASLLaunchableWall::SetupWallPartCollisions(UStaticMeshComponent* WallPart)
{
	if (!WallPart)
	{
		return;
	}
    
	WallPart->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WallPart->SetCollisionObjectType(ECC_WorldDynamic);
	WallPart->SetCollisionResponseToAllChannels(ECR_Block);
	WallPart->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

bool ASLLaunchableWall::ShouldIgnoreCollision(AActor* OtherActor) const
{
	if (!OtherActor)
	{
		return true;
	}
    
	if (ASLAIBaseCharacter* AICharacter = Cast<ASLAIBaseCharacter>(OtherActor))
	{
		if (AICharacter->GetIsDead())
		{
			return true;
		}
	}
    
	if (OtherActor == this)
	{
		return true;
	}
    
	return false;
}
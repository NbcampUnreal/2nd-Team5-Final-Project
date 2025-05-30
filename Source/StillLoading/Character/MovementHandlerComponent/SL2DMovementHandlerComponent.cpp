
// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementHandlerComponent/SL2DMovementHandlerComponent.h"

#include "Character/SLPlayerCharacter.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Character/CameraManagerComponent/CameraManagerComponent.h"
#include "Character/CombatHandlerComponent/CombatHandlerComponent.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Materials/MaterialParameterCollection.h"

USL2DMovementHandlerComponent::USL2DMovementHandlerComponent(): OwnerCharacter(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USL2DMovementHandlerComponent::OnAttackStageFinished(ECharacterMontageState AttackStage)
{
	
}

void USL2DMovementHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ASLPlayerCharacter>(GetOwner());

	if (OwnerCharacter)
	{
		CachedMontageComponent = OwnerCharacter->FindComponentByClass<UAnimationMontageComponent>();
		CachedBattleComponent = OwnerCharacter->FindComponentByClass<UBattleComponent>();
		CachedCombatComponent = OwnerCharacter->FindComponentByClass<UCombatHandlerComponent>();
		BindIMCComponent();
	}

	if (auto* CMC = GetOwner()->FindComponentByClass<UCameraManagerComponent>())
	{
		constexpr EGameCameraType NextType = EGameCameraType::EGCT_TopDown;
		CMC->SwitchCamera(NextType);
	}

	PrimaryComponentTick.bStartWithTickEnabled = true;
	
	// 픽셀 크기 초기화
	if (IsValid(PixelizationMPC))
	{
		PixelSize = FMath::FloorToFloat(1980.0f / PixelizationMPC->GetScalarParameterByName("PixelSize")->DefaultValue);
	}
}

void USL2DMovementHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 이동 중인 경우 목표 위치로 캐릭터 이동
	if (bIsMoving && OwnerCharacter)
	{
		FVector CurrentLocation = OwnerCharacter->GetActorLocation();
		FVector DirectionToTarget = CurrentTargetLocation - CurrentLocation;
		DirectionToTarget.Z = 0.0f;

		// 목표 위치에 도달했거나 거의 도달한 경우
		if (DirectionToTarget.SizeSquared() < 1.0f)
		{
			// 정확한 위치로 설정
			OwnerCharacter->SetActorLocation(CurrentTargetLocation, true);
			bIsMoving = false;
		}
		else
		{
			// 목표 위치를 향해 이동
			float MoveSpeed = PixelSize / 0.1f; // 0.1초 동안 한 픽셀 이동
			float MaxDistanceThisFrame = MoveSpeed * DeltaTime;
			
			if (DirectionToTarget.Size() <= MaxDistanceThisFrame)
			{
				// 이번 프레임에 도착 가능하면 바로 도착
				OwnerCharacter->SetActorLocation(CurrentTargetLocation, true);
				bIsMoving = false;
			}
			else
			{
				// 아직 도착하지 못했으면 해당 방향으로 이동
				DirectionToTarget.Normalize();
				FVector NewLocation = CurrentLocation + DirectionToTarget * MaxDistanceThisFrame;
				OwnerCharacter->SetActorLocation(NewLocation, true);
			}
		}
	}
}

void USL2DMovementHandlerComponent::BindIMCComponent()
{
	if (auto* IMC = GetOwner()->FindComponentByClass<UDynamicIMCComponent>())
	{
		IMC->OnActionTriggered.AddDynamic(this, &ThisClass::OnActionTriggered);
		IMC->OnActionStarted.AddDynamic(this, &ThisClass::OnActionStarted);
		IMC->OnActionCompleted.AddDynamic(this, &ThisClass::OnActionCompleted);
	}
}

void USL2DMovementHandlerComponent::OnActionTriggered(EInputActionType ActionType, FInputActionValue Value)
{
	switch (ActionType)
	{
	case EInputActionType::EIAT_Look: break;
	case EInputActionType::EIAT_MoveUp:
	case EInputActionType::EIAT_MoveDown:
	case EInputActionType::EIAT_MoveLeft:
	case EInputActionType::EIAT_MoveRight:
		Move(Value.Get<float>(), ActionType);
		break;

	default:
		checkNoEntry();
		break;
	}
}

void USL2DMovementHandlerComponent::OnActionStarted(EInputActionType ActionType)
{
}

void USL2DMovementHandlerComponent::OnActionCompleted(EInputActionType ActionType)
{
}

void USL2DMovementHandlerComponent::Move(const float AxisValue, const EInputActionType ActionType)
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_MovementBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Movement Blocked"));
		return;
	}
	
	if (!OwnerCharacter || FMath::IsNearlyZero(AxisValue)) return;

	AController* Controller = OwnerCharacter->GetController();
	if (!Controller) return;

	FRotator ControlRotation = Controller->GetControlRotation();
	FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

	FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	ForwardDir.Z = 0.f;
	RightDir.Z = 0.f;
	ForwardDir.Normalize();
	RightDir.Normalize();

	switch (ActionType)
	{
	case EInputActionType::EIAT_MoveUp:
		OwnerCharacter->AddMovementInput(ForwardDir, AxisValue);
		break;
	case EInputActionType::EIAT_MoveDown:
		OwnerCharacter->AddMovementInput(-ForwardDir, AxisValue);
		break;
	case EInputActionType::EIAT_MoveLeft:
		OwnerCharacter->AddMovementInput(-RightDir, AxisValue);
		break;
	case EInputActionType::EIAT_MoveRight:
		OwnerCharacter->AddMovementInput(RightDir, AxisValue);
		break;
	default:
		break;
	}
}

void USL2DMovementHandlerComponent::StartPixelMovement(const FVector& Direction)
{
	if (!IsValid(PixelizationMPC) || !OwnerCharacter)
	{
		return;
	}

	// 현재 위치에서 픽셀 단위로 이동할 위치 계산
	FVector CurrentLocation = OwnerCharacter->GetActorLocation();
	FVector Offset = Direction * PixelSize;
	FVector TargetLocation = CurrentLocation + Offset;

	// 픽셀 그리드에 맞추기
	TargetLocation.X = FMath::GridSnap(TargetLocation.X, PixelSize);
	TargetLocation.Y = FMath::GridSnap(TargetLocation.Y, PixelSize);

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	FVector Velocity = (TargetLocation - CurrentLocation) / DeltaTime;
	OwnerCharacter->GetCharacterMovement()->Velocity = Velocity;

	UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Start Pixel Movement %f %f %f"), TargetLocation.X, TargetLocation.Y, Velocity.Size());
	
	// 이동 상태 설정
	CurrentTargetLocation = TargetLocation;
	MoveDirection = Direction;
	bIsMoving = true;
}

void USL2DMovementHandlerComponent::MoveByPixelUnit(const FVector& Direction, float AxisValue)
{
	check(IsValid(PixelizationMPC));

	PixelSize = FMath::FloorToFloat(1980.0f / PixelizationMPC->GetScalarParameterByName("PixelSize")->DefaultValue);
	const FVector Offset = Direction * PixelSize;

	FVector CurrentLocation = OwnerCharacter->GetActorLocation();
	FVector TargetLocation = CurrentLocation + Offset;

	TargetLocation.X = FMath::GridSnap(TargetLocation.X, PixelSize);
	TargetLocation.Y = FMath::GridSnap(TargetLocation.Y, PixelSize);

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	FVector Velocity = (TargetLocation - CurrentLocation) / DeltaTime;

	OwnerCharacter->GetCharacterMovement()->Velocity = Velocity;
	OwnerCharacter->SetActorLocation(TargetLocation, true);
}
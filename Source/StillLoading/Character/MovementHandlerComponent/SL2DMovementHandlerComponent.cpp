// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementHandlerComponent/SL2DMovementHandlerComponent.h"

#include "Character/SLBaseCharacter.h"
#include "Character/SLCharacter.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Character/Buffer/InputBufferComponent.h"
#include "Character/CameraManagerComponent/CameraManagerComponent.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Character/PlayerState/SLBattlePlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
USL2DMovementHandlerComponent::USL2DMovementHandlerComponent() : OwnerCharacter(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USL2DMovementHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ASLCharacter>(GetOwner());

	if (OwnerCharacter)
	{
		BindIMCComponent();
		EnableRetroMovement();
		SetTopDownView();
	}
	
}

void USL2DMovementHandlerComponent::OnActionTriggered(EInputActionType ActionType, FInputActionValue Value)
{
	switch (ActionType)
	{
	case EInputActionType::EIAT_MoveUp:
	case EInputActionType::EIAT_MoveDown:
	case EInputActionType::EIAT_MoveLeft:
	case EInputActionType::EIAT_MoveRight:
		Move(Value.Get<float>(), ActionType);
		//Move(Value.Get<float>(), ActionType);
		break;

	default:
		break;
	}
}

void USL2DMovementHandlerComponent::OnActionStarted(EInputActionType ActionType)
{
	switch (ActionType)
	{
	case EInputActionType::EIAT_Interaction:
		Interact();
		break;
	case EInputActionType::EIAT_Attack:
		if (UInputBufferComponent* BufferComp = GetOwner()->FindComponentByClass<UInputBufferComponent>())
		{
			BufferComp->AddBufferedInput(ActionType);
		}
		//Attack();
		break;
	case EInputActionType::EIAT_Menu:
		ToggleMenu();
		break;

	default:
		break;
	}
}

void USL2DMovementHandlerComponent::OnActionCompleted(EInputActionType ActionType)
{
}

void USL2DMovementHandlerComponent::BindIMCComponent()
{
	if (auto* IMC = GetOwner()->FindComponentByClass<UDynamicIMCComponent>())
	{
		IMC->OnActionTriggered.AddDynamic(this, &USL2DMovementHandlerComponent::OnActionTriggered);
		IMC->OnActionStarted.AddDynamic(this, &USL2DMovementHandlerComponent::OnActionStarted);
	}
}


// Called every frame
void USL2DMovementHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);



	// ...
}

void USL2DMovementHandlerComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	DisableRetroMovement();
	UE_LOG(LogTemp, Warning, TEXT("2DMovementHandlerComponent destroyed!"));

}

void USL2DMovementHandlerComponent::EnableRetroMovement()
{
	checkf(OwnerCharacter, TEXT("OwnerCharacter is null"));

	OriginalMaxWalkSpeed = OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed;
	OriginalBrakingFrictionFactor = OwnerCharacter->GetCharacterMovement()->BrakingFrictionFactor;
	OriginalGravityScale = OwnerCharacter->GetCharacterMovement()->GravityScale;
	OriginalGroundFriction = OwnerCharacter->GetCharacterMovement()->GroundFriction;
	bOrigianlUseControllerDesiredRotation = OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation;
	//OriginalMovementMode = OwnerCharacter->GetCharacterMovement()->

	OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = 0.0f;
	OwnerCharacter->GetCharacterMovement()->BrakingFrictionFactor = 0.0f;
	OwnerCharacter->GetCharacterMovement()->GravityScale = 0.0f;
	OwnerCharacter->GetCharacterMovement()->GroundFriction = 0.f;
	OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	//OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking); // or MOVE_Custom
	//OwnerCharacter->GetCharacterMovement()->SetPlaneConstraintEnabled(true);
	//OwnerCharacter->GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Z); // Z 고정, XY 이동

	
	UE_LOG(LogTemp, Warning, TEXT("EnableRetroMovement succeed"));

}

void USL2DMovementHandlerComponent::DisableRetroMovement()
{
	checkf(OwnerCharacter, TEXT("OwnerCharacter is null"));

	OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = OriginalMaxWalkSpeed;
	OwnerCharacter->GetCharacterMovement()->BrakingFrictionFactor = OriginalBrakingFrictionFactor;
	OwnerCharacter->GetCharacterMovement()->GravityScale = OriginalGravityScale;

	UE_LOG(LogTemp, Warning, TEXT("DisableRetroMovement succeed"));
}

void USL2DMovementHandlerComponent::Look(const FVector2D& Value)
{
}

void USL2DMovementHandlerComponent::Move(const float AxisValue, const EInputActionType ActionType)
{
	if (!OwnerCharacter || FMath::IsNearlyZero(AxisValue)) return;

	//AController* Controller = OwnerCharacter->GetController();
	//if (!Controller) return;

	//FRotator ControlRotation = Controller->GetControlRotation();
	//FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

	//FVector ForwardDir = FVector::ForwardVector;
		//FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	//FVector RightDir = FVector::RightVector;
		//FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	//ForwardDir.Z = 0.f;
	//RightDir.Z = 0.f;
	//ForwardDir.Normalize();
	//RightDir.Normalize();

	switch (ActionType)
	{
	case EInputActionType::EIAT_MoveUp:
		MoveInDirection(FVector::ForwardVector);
		//OwnerCharacter->AddMovementInput(ForwardDir, AxisValue);
		break;
	case EInputActionType::EIAT_MoveDown:
		MoveInDirection(-FVector::ForwardVector);

		//OwnerCharacter->AddMovementInput(-ForwardDir, AxisValue);
		break;
	case EInputActionType::EIAT_MoveLeft:
		MoveInDirection(-FVector::RightVector);

		//OwnerCharacter->AddMovementInput(-RightDir, AxisValue);
		break;
	case EInputActionType::EIAT_MoveRight:
		MoveInDirection(FVector::RightVector);
		//OwnerCharacter->AddMovementInput(RightDir, AxisValue);
		break;
	default:
		break;
	}
}

void USL2DMovementHandlerComponent::MoveInDirection(FVector Direction)
{
	if (bIsMoving) return;

	const float StepSize = 100.0f;
	FVector TargetLocation = OwnerCharacter->GetActorLocation() + Direction * StepSize;

	RotateToDirection(Direction);

	if (!CanMoveToLocation(TargetLocation))
	{
		return;
	}

	bIsMoving = true;
	// 위치 스냅: 격자 정렬
	TargetLocation.X = FMath::GridSnap(TargetLocation.X, StepSize);
	TargetLocation.Y = FMath::GridSnap(TargetLocation.Y, StepSize);

	// 일정 시간 후 완료 처리
	FTimerHandle MoveTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(MoveTimerHandle, [this, TargetLocation]()
		{
			OwnerCharacter->SetActorLocation(TargetLocation);
			bIsMoving = false;
		}, 0.5f, false);
}

void USL2DMovementHandlerComponent::RotateToDirection(FVector Direction)
{
	if (Direction.IsNearlyZero()) return;

	FRotator TargetRotation = Direction.Rotation(); // 방향을 회전 값으로 변환
	TargetRotation.Pitch = 0.f; // 기울임 제거
	TargetRotation.Roll = 0.f;

	OwnerCharacter->SetActorRotation(TargetRotation); // 즉시 회전
}

bool USL2DMovementHandlerComponent::CanMoveToLocation(FVector TargetLocation)
{

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter); // 자기 자신은 제외

	FVector Start = OwnerCharacter->GetActorLocation();
	FVector End = TargetLocation;

	// SweepSphere로 충돌 확인
	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		Start,
		End,
		FQuat::Identity,
		ECC_WorldStatic, // 충돌 채널: 고정 오브젝트
		FCollisionShape::MakeSphere(10.f), // 반지름 설정 (캐릭터 크기보다 살짝 작게)
		Params
	);

	return !bHit;
}

void USL2DMovementHandlerComponent::Interact()
{
	// TODO: 인터랙션 대상 탐색 및 처리

}

void USL2DMovementHandlerComponent::ToggleMenu()
{
	UE_LOG(LogTemp, Log, TEXT("Menu opened or closed"));
	// TODO: UI 호출 / Input 모드 변경 등 처리

}

void USL2DMovementHandlerComponent::SetTopDownView()
{
	if (auto* CMC = GetOwner()->FindComponentByClass<UCameraManagerComponent>())
	{
		const EGameCameraType NextType = EGameCameraType::EGCT_TopDown;
		CMC->SwitchCamera(NextType);
	}
}

void USL2DMovementHandlerComponent::Attack()
{
	UE_LOG(LogTemp, Log, TEXT("Attack triggered"));

	// TODO: 무기/애니메이션 처리 연결
	if (auto* BC = GetOwner()->FindComponentByClass<UBattleComponent>())
	{
		BC->PerformAttack();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BattleComponent not found on %s"), *GetOwner()->GetName());
	}
}


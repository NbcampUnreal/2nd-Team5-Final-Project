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
	check(OwnerCharacter); // 캐릭터가 아닌 경우 에러

	BindIMCComponent();
	SetTopDownView();


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

	if (bIsMoving)
	{
		MoveElapsed += DeltaTime;
		float Alpha = FMath::Clamp(MoveElapsed / MoveDuration, 0.0f, 1.0f);

		FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
		OwnerCharacter->SetActorLocation(NewLocation);

		if (Alpha >= 1.0f)
		{
			bIsMoving = false;

			// 다음 이동 처리
			if (NextMove != FVector::ZeroVector)
			{
				FVector NextDir = NextMove;
				NextMove = FVector::ZeroVector;
				MoveGrid(NextDir);
			}
		}
	}
}

void USL2DMovementHandlerComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	UE_LOG(LogTemp, Warning, TEXT("2DMovementHandlerComponent destroyed!"));

}




void USL2DMovementHandlerComponent::Look(const FVector2D& Value)
{
}

void USL2DMovementHandlerComponent::Move(const float AxisValue, const EInputActionType ActionType)
{
	if (!OwnerCharacter || FMath::IsNearlyZero(AxisValue))
	{
		return;
	}

	switch (ActionType)
	{
	case EInputActionType::EIAT_MoveUp:
		MoveGrid(FVector::ForwardVector);
		break;

	case EInputActionType::EIAT_MoveDown:
		MoveGrid(-FVector::ForwardVector);
		break;

	case EInputActionType::EIAT_MoveLeft:
		MoveGrid(-FVector::RightVector);
		break;

	case EInputActionType::EIAT_MoveRight:
		MoveGrid(FVector::RightVector);
		break;

	default:
		break;
	}
}

void USL2DMovementHandlerComponent::MoveGrid(FVector InputDir)
{
	if (bIsMoving)
	{
		NextMove = InputDir;
		return;
	}

	StartLocation = OwnerCharacter->GetActorLocation();

	TargetLocation = StartLocation + InputDir.GetSafeNormal() * StepDistance;

	MoveElapsed = 0.0f;
	 
	bIsMoving = true;

}




void USL2DMovementHandlerComponent::RotateToDirection(FVector Direction)
{
	if (Direction.IsNearlyZero()) return;

	FRotator TargetRotation = Direction.Rotation(); // 방향을 회전 값으로 변환
	TargetRotation.Pitch = 0.f; // 기울임 제거
	TargetRotation.Roll = 0.f;

	OwnerCharacter->SetActorRotation(TargetRotation); // 즉시 회전
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


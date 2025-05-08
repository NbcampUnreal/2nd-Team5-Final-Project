#include "SLMovementHandlerComponent.h"

#include "Character/SLBaseCharacter.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UMovementHandlerComponent::UMovementHandlerComponent(): OwnerCharacter(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMovementHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	BindIMCComponent();
}

void UMovementHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMovementHandlerComponent::OnActionTriggered(EInputActionType ActionType)
{
	FString EnumName = StaticEnum<EInputActionType>()->GetNameStringByValue(static_cast<int64>(ActionType));
	EnumName.RemoveFromStart("EInputActionType::");

	UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent::OnActionTriggered → %s"), *EnumName);
	
	switch (ActionType)
	{
	case EInputActionType::Jump:
		Jump();
		break;

	case EInputActionType::MoveUp:
		Move(FVector::ForwardVector);
		break;

	case EInputActionType::MoveDown:
		Move(-FVector::ForwardVector);
		break;

	case EInputActionType::MoveLeft:
		Move(-FVector::RightVector);
		break;

	case EInputActionType::MoveRight:
		Move(FVector::RightVector);
		break;

	case EInputActionType::Interaction:
		Interact();
		break;

	case EInputActionType::Attack:
		Attack();
		break;

	case EInputActionType::PointMove:
		PointMove();
		break;

	case EInputActionType::Walk:
		ToggleWalk();
		break;

	case EInputActionType::Menu:
		ToggleMenu();
		break;

	default:
		break;
	}
}

void UMovementHandlerComponent::BindIMCComponent()
{
	if (auto* IMC = GetOwner()->FindComponentByClass<UDynamicIMCComponent>())
	{
		IMC->OnActionTriggered.AddDynamic(this, &UMovementHandlerComponent::OnActionTriggered);
	}
}

void UMovementHandlerComponent::Jump()
{
	UE_LOG(LogTemp, Warning, TEXT("Jump"));
	if (OwnerCharacter) OwnerCharacter->Jump();
}

void UMovementHandlerComponent::Move(const FVector& Direction)
{
	if (OwnerCharacter) OwnerCharacter->AddMovementInput(Direction, 1.0f);

	if (UCharacterMovementComponent* CharacterMovementComp = OwnerCharacter->GetCharacterMovement())
	{
		//CharacterMovementComp->MaxWalkSpeed = ;
	}
}

void UMovementHandlerComponent::Interact()
{
	UE_LOG(LogTemp, Log, TEXT("Interact triggered"));
	// TODO: 인터랙션 대상 탐색 및 처리
	
}

void UMovementHandlerComponent::Attack()
{
	UE_LOG(LogTemp, Log, TEXT("Attack triggered"));
	// TODO: 무기/애니메이션 처리 연결
	
}

void UMovementHandlerComponent::PointMove()
{
	UE_LOG(LogTemp, Log, TEXT("PointMove triggered"));
	// TODO: 마우스 클릭 위치로 이동 (RTS 스타일)
	
}

void UMovementHandlerComponent::ToggleWalk()
{
	if (OwnerCharacter)
	{
		
	}
}

void UMovementHandlerComponent::ToggleMenu()
{
	UE_LOG(LogTemp, Log, TEXT("Menu opened or closed"));
	// TODO: UI 호출 / Input 모드 변경 등 처리
	
}
#include "SLCompanionFlyingComponent.h"
#include "AI/Companion/SLCompanionCharacter.h"
#include "AI/SLAIFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/SLBaseAIController.h"

USLCompanionFlyingComponent::USLCompanionFlyingComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    DefaultHeight = CompanionConstants::DEFAULT_FLYING_HEIGHT;
    MaxHeight = CompanionConstants::MAX_FLYING_HEIGHT;
    MinHeight = CompanionConstants::MIN_FLYING_HEIGHT;
    bIsFlying = false;
    CurrentHeight = 0.0f;
    TargetHeight = 0.0f;
    CurrentSpeed = CompanionConstants::FLYING_SPEED;
}

void USLCompanionFlyingComponent::BeginPlay()
{
    Super::BeginPlay();
    OwnerCharacter = Cast<ASLCompanionCharacter>(GetOwner());
}

void USLCompanionFlyingComponent::StartFlying()
{
    if (bIsFlying || !OwnerCharacter || OwnerCharacter->GetIsDead())
    {
        return;
    }
    SetFlyingState(true);
    TargetHeight = DefaultHeight;
    CurrentHeight = 0.0f;  // 현재 높이를 0으로 초기화
    
    // 그 다음 비행 상태 설정
    //SetFlyingState(true);
    
    // 타이머 시작
    GetWorld()->GetTimerManager().ClearTimer(FlyingUpdateTimer);
    GetWorld()->GetTimerManager().SetTimer(
        FlyingUpdateTimer, 
        this, 
        &USLCompanionFlyingComponent::UpdateFlyingPosition, 
        CompanionConstants::FLYING_UPDATE_RATE, 
        true
    );
}

void USLCompanionFlyingComponent::StopFlying()
{
    if (!bIsFlying)
    {
        return;
    }
    
    FlyToHeight(0.0f);
    
    FTimerHandle LandingTimer;
    GetWorld()->GetTimerManager().SetTimer(LandingTimer, [this]()
    {
        SetFlyingState(false);
    }, 1.0f, false);
}

void USLCompanionFlyingComponent::FlyToHeight(float NewTargetHeight, float Speed)
{
    if (!OwnerCharacter)
    {
        return;
    }
    
    TargetHeight = FMath::Clamp(NewTargetHeight, 0.0f, MaxHeight);
    CurrentSpeed = Speed;
    
    // 비행 중이 아니고 목표 높이가 0보다 크면
    if (!bIsFlying && TargetHeight > 0.0f)
    {
        // 현재 높이를 0으로 초기화하지 않고 실제 높이를 계산
        FVector CurrentLocation = OwnerCharacter->GetActorLocation();
        FVector GroundLocation = FindGroundPosition(CurrentLocation);
        CurrentHeight = CurrentLocation.Z - GroundLocation.Z;
        
        SetFlyingState(true);
    }
    
    // 타이머가 이미 실행 중이 아닐 때만 시작
    if (!GetWorld()->GetTimerManager().IsTimerActive(FlyingUpdateTimer))
    {
        GetWorld()->GetTimerManager().SetTimer(
            FlyingUpdateTimer, 
            this, 
            &USLCompanionFlyingComponent::UpdateFlyingPosition, 
            CompanionConstants::FLYING_UPDATE_RATE, 
            true
        );
    }
}

void USLCompanionFlyingComponent::UpdateFlyingPosition()
{
    if (!OwnerCharacter || !bIsFlying)
    {
        GetWorld()->GetTimerManager().ClearTimer(FlyingUpdateTimer);
        return;
    }
    
    float HeightDifference = TargetHeight - CurrentHeight;
    
    // 목표 도달 체크
    if (FMath::Abs(HeightDifference) < CompanionConstants::HEIGHT_TOLERANCE)
    {
        CurrentHeight = TargetHeight;
        
        // 목표가 0이고 도달했을 때만 착지
        if (TargetHeight <= 0.0f)
        {
            GetWorld()->GetTimerManager().ClearTimer(FlyingUpdateTimer);
            SetFlyingState(false);
        }
        return;
    }
    
    // 이동 계산
    float DeltaTime = GetWorld()->GetDeltaSeconds();
    float MoveDistance = CurrentSpeed * DeltaTime;
    float MoveDirection = FMath::Sign(HeightDifference);
    
    // 현재 높이 업데이트
    float PreviousHeight = CurrentHeight;
    CurrentHeight += MoveDirection * FMath::Min(MoveDistance, FMath::Abs(HeightDifference));
    
    // 실제 이동
    FVector CurrentLocation = OwnerCharacter->GetActorLocation();
    FVector NewLocation = CurrentLocation;
    NewLocation.Z += (CurrentHeight - PreviousHeight);  // 델타값만큼 이동
    
    // 캐릭터 이동
    OwnerCharacter->SetActorLocation(NewLocation, true);
    
}

void USLCompanionFlyingComponent::SetFlyingState(bool bNewFlyingState)
{
    if (bIsFlying == bNewFlyingState || !OwnerCharacter)
    {
        return;
    }
    
    bIsFlying = bNewFlyingState;
    
    UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
    if (Movement)
    {
        Movement->SetMovementMode(bIsFlying ? MOVE_Flying : MOVE_Walking);
    }
    
    if (ASLBaseAIController* AIController = OwnerCharacter->GetAIController())
    {
        if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
        {
            BlackboardComponent->SetValueAsBool(FName("IsFlying"), bIsFlying);
        }
    }
    
    OnFlyingStateChanged.Broadcast(bIsFlying);
    
    if (!bIsFlying)
    {
        CurrentHeight = 0.0f;
    }
}

FVector USLCompanionFlyingComponent::FindGroundPosition(const FVector& WorldPosition)
{
    FVector GroundCheckLocation = WorldPosition;
    GroundCheckLocation.Z -= CurrentHeight;
    
    return USLAIFunctionLibrary::FindGroundLocation(
        GetWorld(), 
        GroundCheckLocation, 
        2000.0f, 
        OwnerCharacter->GetIsDebugMode()
    );
}
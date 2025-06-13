#include "BTDecorator_IsPathBlocked.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_IsPathBlocked::UBTDecorator_IsPathBlocked()
{
	NodeName = TEXT("Is Path Blocked");
}

bool UBTDecorator_IsPathBlocked::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	if(!bResult) return false;

	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AAIController* AIOwner = OwnerComp.GetAIOwner();

	if (!BlackboardComp || !AIOwner) return false;

	APawn* OwnerPawn = AIOwner->GetPawn();
	if (!OwnerPawn) return false;

	const AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor) return false;

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	
	CollisionParams.AddIgnoredActor(OwnerPawn);
	CollisionParams.AddIgnoredActor(TargetActor);

	const FVector StartPoint = OwnerPawn->GetActorLocation();
	const FVector EndPoint = TargetActor->GetActorLocation();

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartPoint,
		EndPoint,
		ECollisionChannel::ECC_Visibility,
		CollisionParams
	);

#if ENABLE_DRAW_DEBUG
	if (bHit)
	{
		// 장애물에 부딪혔을 경우: 시작점부터 '부딪힌 지점'까지 빨간색 라인을 그립니다.
		DrawDebugLine(
			GetWorld(),
			StartPoint,
			HitResult.Location, // 끝점이 아닌 실제 부딪힌 위치
			FColor::Red,
			false, // bPersistentLines: 라인이 계속 남아있을지 여부
			0.05f, // LifeTime: 라인이 유지되는 시간 (0이면 1프레임)
			0,     // DepthPriority
			1.0f   // Thickness: 라인 두께
		);

		// 부딪힌 지점에 작은 빨간색 구를 그려서 더 명확하게 표시합니다.
		DrawDebugSphere(GetWorld(), HitResult.Location, 12.0f, 12, FColor::Red, false, 0.05f);
	}
	else
	{
		// 장애물이 없을 경우: 시작점부터 목표 지점까지 초록색 라인을 그립니다.
		DrawDebugLine(
			GetWorld(),
			StartPoint,
			EndPoint,
			FColor::Green,
			false,
			0.05f,
			0,
			1.0f
		);
	}
#endif

	return bHit;
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "SLBTTaskRotateToFaceTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

USLBTTaskRotateToFaceTarget::USLBTTaskRotateToFaceTarget()
{
	//이 태스크의 이름 설정
	NodeName = TEXT("Native Rotate to Face Target Actor");
	AnglePrecision = 10.f;		//목표를 향해 회전할 때의 각도 오차를 설정
	RotationInterpSpeed = 5.f;		//회전 속도를 설정
	MaxRotationTime = 3.0f;
	
	bNotifyTick = true;		//프레임(틱)마다 업데이트
	bNotifyTaskFinished = true;	//태스크가 끝났을 때, 완료된 것을 알림
	bCreateNodeInstance = false;	//이 태스크는 새로운 노드 인스턴스를 생성하지 않도록 설정

	INIT_TASK_NODE_NOTIFY_FLAGS();	//태스크 노드에서 필요한 알림 플래그를 초기화

	//InTargetToFaceKey라는 블랙보드키에 AActor만 들어가도록 필터링 설정
	InTargetToFaceKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetToFaceKey), AActor::StaticClass());
}

void USLBTTaskRotateToFaceTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		// AI가 회전해서 바라볼 타겟(목표 액터)을 지정
		InTargetToFaceKey.ResolveSelectedKey(*BBAsset);
	}
}

uint16 USLBTTaskRotateToFaceTarget::GetInstanceMemorySize() const
{
	return sizeof(FRotateToFaceTargetTaskMemory);
}

FString USLBTTaskRotateToFaceTarget::GetStaticDescription() const
{
	const FString KeyDescription = InTargetToFaceKey.SelectedKeyName.ToString();

	return FString::Printf(TEXT("Smoothly rotates to face %s Key until the angle precision: %s is reached"), *KeyDescription, *FString::SanitizeFloat(AnglePrecision));
}

EBTNodeResult::Type USLBTTaskRotateToFaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	//InTargetToFaceKey.SelectedKeyName에 해당하는 객체를 가지고와서 Actor로 형변환
	UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetToFaceKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(ActorObject);
	
	//Ai 폰을 저장
	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

	//태스크의 실행 중에 사용할 메모리를 저장
	FRotateToFaceTargetTaskMemory* Memory = CastInstanceNodeMemory<FRotateToFaceTargetTaskMemory>(NodeMemory);
	check(Memory);

	Memory->OwningPawn = OwningPawn;
	Memory->TargetActor = TargetActor;
	Memory->ElapsedTime = 0.0f;

	//메모리가 유효하지 않으면 작업 실패를 리턴
	if (!Memory->IsValid())
	{
		return EBTNodeResult::Failed;
	}

	//AI 캐릭터가 목표를 향해 설정된 각도 정밀도 내에 있는지 확인
	if (HasReachedAnglePrecision(OwningPawn, TargetActor))
	{
		Memory->Reset();
		//완료
		return EBTNodeResult::Succeeded;
	} 
	//진행중
	return EBTNodeResult::InProgress;
}

void USLBTTaskRotateToFaceTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	FRotateToFaceTargetTaskMemory* Memory = CastInstanceNodeMemory<FRotateToFaceTargetTaskMemory>(NodeMemory);

	// 시간 누적
	Memory->ElapsedTime += DeltaSeconds;

	//메모리의 유효성을 확인
	if (!Memory->IsValid())
	{
		//태스크가 실패로 끝났다고 알림
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 최대 회전 시간을 초과한 경우
	if (Memory->ElapsedTime >= MaxRotationTime)
	{
		Memory->Reset();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	//AI 캐릭터가 목표를 향해 설정된 각도 정밀도 내에 있는지 확인
	if (HasReachedAnglePrecision(Memory->OwningPawn.Get(), Memory->TargetActor.Get()))
	{
		Memory->Reset();
		//태스크가 성공으로 끝났다고 알림
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	//회전이 완료되지 않은 경우
	//타겟을 보도록 회전
	const FRotator CurrentRot = Memory->OwningPawn->GetActorRotation();
	const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
		Memory->OwningPawn->GetActorLocation(), 
		Memory->TargetActor->GetActorLocation()
	);
	FRotator TargetRot = FMath::RInterpTo(CurrentRot, LookAtRot, DeltaSeconds, RotationInterpSpeed);

	// 위아래 회전(Pitch)은 그대로 유지하도록 현재 값으로 설정
	TargetRot.Pitch = CurrentRot.Pitch;

	UE_LOG(LogTemp, Warning, TEXT("Current: %s, Target: %s, Setting: %s"), 
		   *CurrentRot.ToString(), *LookAtRot.ToString(), *TargetRot.ToString());
	
	Memory->OwningPawn->SetActorRotation(TargetRot);
}

bool USLBTTaskRotateToFaceTarget::HasReachedAnglePrecision(APawn* QueryPawn, AActor* TargetActor) const
{
	if (!QueryPawn || !TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("QueryPawn 또는 TargetActor가 nullptr입니다."));
		return false;
	}
    
	// Yaw 회전만 체크하도록 수정
	const FVector PawnLocation = QueryPawn->GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();
    
	// Z축을 무시하고 2D 평면에서의 방향 벡터 계산
	FVector ToTarget2D = TargetLocation - PawnLocation;
	ToTarget2D.Z = 0.0f;
    
	if (ToTarget2D.IsNearlyZero())
	{
		return true;
	}
    
	// 정규화
	ToTarget2D.Normalize();
    
	// 폰의 전방 벡터도 2D로 변환
	FVector OwnerForward2D = QueryPawn->GetActorForwardVector();
	OwnerForward2D.Z = 0.0f;
	OwnerForward2D.Normalize();
    
	// 내적 계산
	const float DotResult = FMath::Clamp(FVector::DotProduct(OwnerForward2D, ToTarget2D), -1.0f, 1.0f);
    
	// 각도 계산
	const float AngleDiff = UKismetMathLibrary::DegAcos(DotResult);
    
	return AngleDiff <= AnglePrecision;
}

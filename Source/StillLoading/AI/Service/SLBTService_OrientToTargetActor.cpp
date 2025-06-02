// Fill out your copyright notice in the Description page of Project Settings.


#include "SLBTService_OrientToTargetActor.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

USLBTService_OrientToTargetActor::USLBTService_OrientToTargetActor()
{
    //서비스 노드의 이름을 설정
    NodeName = TEXT("Native Orient Rotation To Target Actor");
    //서비스 노드의 기본 설정을 초기화
    INIT_SERVICE_NODE_NOTIFY_FLAGS();
    // AI가 목표 액터를 바라볼 때 회전하는 속도를 설정
    RotationInterpSpeed = 5.f;
    //서비스가 실행되는 간격, 매 프레임마다 호출
    Interval = 0.f;
    //호출 간격에 추가될 무작위 편차, 0이므로 편차가 없음
    RandomDeviation = 0.f;

    //블랙보드 키가 가리킬 수 있는 값의 타입을 제한, 현재 클래스에 있는 InTargetActorKey키가  반드시 AActor만을 참조하도록 제한
    //this 매서드가 호출된 클래스, GET_MEMBER_NAME_CHECKED 현재 클래스에서 InTargetActorKey변수를 안전하게 가지고 옴. 
    InTargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetActorKey), AActor::StaticClass());
}

void USLBTService_OrientToTargetActor::InitializeFromAsset(UBehaviorTree& Asset)
{
    Super::InitializeFromAsset(Asset);

    //함수는 현재의 행동 트리에 연결된 블랙보드 데이터
    if (UBlackboardData* BBAsset = GetBlackboardAsset())
    {
        //블랙보드에서 키를 연결해줌
        InTargetActorKey.ResolveSelectedKey(*BBAsset);
    }
}

FString USLBTService_OrientToTargetActor::GetStaticDescription() const
{
    // InTargetActorKey.SelectedKeyName의 이름을 문자열로 변환
    const FString KeyDescription = InTargetActorKey.SelectedKeyName.ToString();

    //GetStaticServiceDescription() : 서비스 노드의 기본 설명을 반환
    return FString::Printf(TEXT("Orient rotation to %s Key %s"), *KeyDescription, *GetStaticServiceDescription());
}

void USLBTService_OrientToTargetActor::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    //타겟 액터에 해당하는 오브젝트를 액터로 형변환
    UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetActorKey.SelectedKeyName);
    AActor* TargetActor = Cast<AActor>(ActorObject);

    //AI를 소유한 폰을 가지고옴
    APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

    //타겟 액터와 AI폰이 유효하다면
    if (OwningPawn && TargetActor)
    {
        //AI 폰에서 타겟 액터까지의 회전값을 구함
        const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(OwningPawn->GetActorLocation(), TargetActor->GetActorLocation());
        
        //현재 회전값 저장
        const FRotator CurrentRot = OwningPawn->GetActorRotation();

        //AI 폰이 부드럽게 회전하게 함
        FRotator TargetRot = FMath::RInterpTo(CurrentRot, LookAtRot, DeltaSeconds, RotationInterpSpeed);
        
        // 위아래 회전(Pitch)은 현재 값으로 유지
        TargetRot.Pitch = CurrentRot.Pitch;

        //회전 설정
        OwningPawn->SetActorRotation(TargetRot);
    }
}

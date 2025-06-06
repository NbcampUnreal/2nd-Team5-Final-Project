// SLMultiHitNotifyState.cpp
#include "SLMultiHitNotifyState.h"
#include "Character/SLAIBaseCharacter.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"

FHitShapeData::FHitShapeData()
{
	ShapeType = EHitShapeType::EHST_Capsule;
	SphereRadius = 50.0f;
	CapsuleRadius = 50.0f;
	CapsuleHalfHeight = 100.0f;
	BoxExtent = FVector(50.0f, 50.0f, 50.0f);
	LocationOffset = FVector(0.0f, 0.0f, 50.0f);
	RotationOffset = FRotator::ZeroRotator;
	SweepDistance = 100.0f;
}

USLMultiHitNotifyState::USLMultiHitNotifyState()
{
	HitInterval = 0.2f;
	MaxHitCount = 3;
	AttackType = EAttackAnimType::AAT_Attack_01;
	bShowDebug = false;
	DebugDrawDuration = 1.0f;
	CurrentHitCount = 0;
	TimeUntilNextHit = 0.0f;
}

void USLMultiHitNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	ASLAIBaseCharacter* Character = Cast<ASLAIBaseCharacter>(MeshComp->GetOwner());
	if (!Character)
	{
		return;
	}

	UBattleComponent* BattleComp = Character->GetBattleComponent();
	if (!BattleComp)
	{
		return;
	}

	// 초기화
	CurrentHitCount = 0;
	TimeUntilNextHit = 0.0f;
	HitActorsThisFrame.Empty();

	// 공격 타입 설정
	Character->SetCurrentAttackType(AttackType);

	// 이전 히트 대상 초기화
	BattleComp->ClearHitTargets();

	// 첫 번째 히트 즉시 실행
	PerformHit(Character, BattleComp);

	if (bShowDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("MultiHit NotifyBegin - MaxHits: %d, Interval: %f"), MaxHitCount, HitInterval);
	}
}

void USLMultiHitNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	ASLAIBaseCharacter* Character = Cast<ASLAIBaseCharacter>(MeshComp->GetOwner());
	if (!Character)
	{
		return;
	}

	UBattleComponent* BattleComp = Character->GetBattleComponent();
	if (!BattleComp)
	{
		return;
	}

	// 최대 히트 횟수에 도달했으면 종료
	if (CurrentHitCount >= MaxHitCount)
	{
		return;
	}

	// 다음 히트 시간 계산
	TimeUntilNextHit -= FrameDeltaTime;

	// 히트 실행
	if (TimeUntilNextHit <= 0.0f)
	{
		PerformHit(Character, BattleComp);
		TimeUntilNextHit = HitInterval;
	}
}

void USLMultiHitNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	ASLAIBaseCharacter* Character = Cast<ASLAIBaseCharacter>(MeshComp->GetOwner());
	if (!Character)
	{
		return;
	}

	// 히트 대상 초기화
	if (UBattleComponent* BattleComp = Character->GetBattleComponent())
	{
		BattleComp->ClearHitTargets();
	}

	HitActorsThisFrame.Empty();

	if (bShowDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("MultiHit NotifyEnd - Total Hits: %d"), CurrentHitCount);
	}
}

FString USLMultiHitNotifyState::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("MultiHit [%s] x%d"), 
		*UEnum::GetValueAsString(AttackType), 
		MaxHitCount);
}

void USLMultiHitNotifyState::PerformHit(ASLAIBaseCharacter* Character, UBattleComponent* BattleComp)
{
	if (!Character || !BattleComp)
	{
		return;
	}

	// 히트 대상 초기화 (매 히트마다 같은 대상을 다시 때릴 수 있도록)
	BattleComp->ClearHitTargets();
	HitActorsThisFrame.Empty();

	// 커스텀 스윕 실행
	DoCustomSweep(Character, BattleComp);

	CurrentHitCount++;

	if (bShowDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("MultiHit Performed - Hit #%d"), CurrentHitCount);
	}
}

void USLMultiHitNotifyState::DoCustomSweep(ASLAIBaseCharacter* Character, UBattleComponent* BattleComp)
{
	if (!Character || !BattleComp || !Character->GetWorld())
	{
		return;
	}

	UWorld* World = Character->GetWorld();

	// 시작 위치 계산
	FVector CharacterLocation = Character->GetActorLocation();
	FRotator CharacterRotation = Character->GetActorRotation();
	
	// 오프셋 적용
	FVector Start = CharacterLocation + CharacterRotation.RotateVector(HitShape.LocationOffset);
	FVector End = Start + CharacterRotation.RotateVector(FVector(HitShape.SweepDistance, 0.0f, 0.0f));
	
	// 회전 적용
	FQuat SweepRotation = (CharacterRotation + HitShape.RotationOffset).Quaternion();

	// 콜리전 쉐이프 생성
	FCollisionShape CollisionShape = GetCollisionShape();

	// 히트 결과 저장
	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);
	QueryParams.bTraceComplex = false;

	// 스윕 실행
	World->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		SweepRotation,
		ECC_GameTraceChannel1,
		CollisionShape,
		QueryParams
	);

	// 디버그 그리기
	if (bShowDebug)
	{
		FColor DebugColor = FColor::Red;
		
		switch (HitShape.ShapeType)
		{
		case EHitShapeType::EHST_Sphere:
			DrawDebugSphere(World, End, HitShape.SphereRadius, 16, DebugColor, false, DebugDrawDuration);
			break;
			
		case EHitShapeType::EHST_Capsule:
			DrawDebugCapsule(World, End, HitShape.CapsuleHalfHeight, HitShape.CapsuleRadius, 
				SweepRotation, DebugColor, false, DebugDrawDuration);
			break;
			
		case EHitShapeType::EHST_Box:
			DrawDebugBox(World, End, HitShape.BoxExtent, SweepRotation, DebugColor, false, DebugDrawDuration);
			break;
		}
		
		// 스윕 경로 표시
		DrawDebugLine(World, Start, End, FColor::Yellow, false, DebugDrawDuration, 0, 2.0f);
	}

	// 히트 처리
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		
		if (HitActor && HitActor != Character && !HitActorsThisFrame.Contains(HitActor))
		{
			if (UBattleComponent* TargetBattleComp = HitActor->FindComponentByClass<UBattleComponent>())
			{
				HitActorsThisFrame.Add(HitActor);
				BattleComp->SendHitResult(HitActor, Hit, AttackType);
				
				if (bShowDebug)
				{
					UE_LOG(LogTemp, Warning, TEXT("MultiHit - Hit Actor: %s"), *HitActor->GetName());
					DrawDebugPoint(World, Hit.ImpactPoint, 10.0f, FColor::Green, false, DebugDrawDuration);
				}
			}
		}
	}
}

FCollisionShape USLMultiHitNotifyState::GetCollisionShape() const
{
	switch (HitShape.ShapeType)
	{
	case EHitShapeType::EHST_Sphere:
		return FCollisionShape::MakeSphere(HitShape.SphereRadius);
		
	case EHitShapeType::EHST_Capsule:
		return FCollisionShape::MakeCapsule(HitShape.CapsuleRadius, HitShape.CapsuleHalfHeight);
		
	case EHitShapeType::EHST_Box:
		return FCollisionShape::MakeBox(HitShape.BoxExtent);
		
	default:
		return FCollisionShape::MakeCapsule(50.0f, 100.0f);
	}
}
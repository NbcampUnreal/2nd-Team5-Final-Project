// SLBossCharacter.cpp
#include "SLBossCharacter.h"

#include "AIController.h"
#include "AnimInstances/SLAICharacterAnimInstance.h"
#include "AnimInstances/SLBossAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/SLBaseAIController.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"


ASLBossCharacter::ASLBossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	BossAttackPattern = EBossAttackPattern::EBAP_None;
	LastLongRangePattern = EBossAttackPattern::EBAP_None;

	MaxHealth = 100.0f;
	
	// Material 관련 초기화
	AllMeshesTargetAlpha = 1.0f;
	AllMeshesFadeStartTime = 0.0f;
	AllMeshesFadeDuration = 1.0f;

	//처형 안되게 수정해야함
	bCanBeExecuted = true;
}

void ASLBossCharacter::SetBossAttackPattern(EBossAttackPattern NewPattern)
{
	BossAttackPattern = NewPattern;

	if (USLBossAnimInstance* SLAIAnimInstance = Cast<USLBossAnimInstance>(AnimInstancePtr.Get()))
	{
		SLAIAnimInstance->SetBossAttackPattern(NewPattern);
	}
}

void ASLBossCharacter::FindTargetPoint()
{
	// 레벨에서 TargetPoint 액터 찾기
	TArray<AActor*> FoundTargetPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), FoundTargetPoints);
    
	// 찾은 TargetPoint가 있으면 설정
	if (FoundTargetPoints.Num() > 0)
	{
		TargetPoint = FoundTargetPoints[0];
		UE_LOG(LogTemp, Display, TEXT("Found TargetPoint: %s"), *TargetPoint->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No TargetPoint found in level!"));
	}
}

void ASLBossCharacter::SetTargetPointToBlackboard()
{
	if (!TargetPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("No TargetPoint set. Call FindTargetPoint first."));
		return;
	}
    
	if (AIController)
	{
		UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
		if (Blackboard)
		{
			Blackboard->SetValueAsObject("TargetPoint", TargetPoint);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AIController Is null"));
	}
}

void ASLBossCharacter::BeginPlay()
{
	Super::BeginPlay();

	FindTargetPoint();
	SetTargetPointToBlackboard();

	
	CurrentHealth = MaxHealth;
}

EBossAttackPattern ASLBossCharacter::SelectRandomPattern(float DistanceToTarget, const TArray<EBossAttackPattern>& CloseRangePatterns, const TArray<EBossAttackPattern>& LongRangePatterns, float DistanceThreshold)
{
    if (DistanceToTarget >= DistanceThreshold)
    {
        // 원거리 패턴
        TArray<EBossAttackPattern> AvailableLongRangePatterns = LongRangePatterns;
        
        // 이전에 사용한 패턴이 있고, 목록에 포함되어 있다면 제거
        if (LastLongRangePattern != EBossAttackPattern::EBAP_None && 
            AvailableLongRangePatterns.Contains(LastLongRangePattern))
        {
            AvailableLongRangePatterns.Remove(LastLongRangePattern);
        }
        
        // 사용 가능한 패턴이 없다면 전체 목록에서 선택
        if (AvailableLongRangePatterns.Num() == 0)
        {
            AvailableLongRangePatterns = LongRangePatterns;
        }
        
        // 랜덤 선택
        int32 RandomIndex = FMath::RandRange(0, AvailableLongRangePatterns.Num() - 1);
        EBossAttackPattern SelectedPattern = AvailableLongRangePatterns[RandomIndex];
        
        LastLongRangePattern = SelectedPattern;
        
        UE_LOG(LogTemp, Display, TEXT("Selected long range pattern: %s"), *UEnum::GetValueAsString(SelectedPattern));
        return SelectedPattern;
    }
    else
    {
    	if (USLAICharacterAnimInstance* AnimInstance = Cast<USLAICharacterAnimInstance>(AnimInstancePtr.Get()))
    	{
    		if (AnimInstance->IsTargetBehindCharacter(120.0f)) // 90도 각도로 뒤에 있는지 확인
    		{
    			return EBossAttackPattern::EBAP_Attack_04;
    		}
    	}
    
        // 근거리 패턴
        if (CloseRangePatterns.Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("No close range patterns available!"));
            return EBossAttackPattern::EBAP_Attack_01; // 기본값
        }
        
        int32 RandomIndex = FMath::RandRange(0, CloseRangePatterns.Num() - 1);
        EBossAttackPattern SelectedPattern = CloseRangePatterns[RandomIndex];
        
        return SelectedPattern;
    }
}

void ASLBossCharacter::SetupChildMeshesMaterials(const TArray<USkeletalMeshComponent*>& ChildMeshes, int32 MaterialIndex)
{
	// 기존 데이터 초기화
	ChildMeshesDynamicMaterials.Empty();
	ChildMeshComponents.Empty();
	ChildMeshesCurrentAlpha.Empty();
	ChildMeshesStartAlpha.Empty();

	for (USkeletalMeshComponent* ChildMesh : ChildMeshes)
	{
		if (!ChildMesh)
		{
			continue;
		}

		// 머티리얼 가져오기
		UMaterialInterface* SourceMaterial = ChildMesh->GetMaterial(MaterialIndex);
		if (!SourceMaterial)
		{
			UE_LOG(LogTemp, Warning, TEXT("No material found at index %d for mesh %s"), MaterialIndex, *ChildMesh->GetName());
			continue;
		}

		// Dynamic Material Instance 생성
		UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(SourceMaterial, this);
		if (DynMaterial)
		{
			// 머티리얼 적용
			ChildMesh->SetMaterial(MaterialIndex, DynMaterial);
			
			// 초기 Alpha 값 설정
			DynMaterial->SetScalarParameterValue(FName("Alpha"), 0.0f);
			
			// 배열에 저장
			ChildMeshesDynamicMaterials.Add(DynMaterial);
			ChildMeshComponents.Add(ChildMesh);
			ChildMeshesCurrentAlpha.Add(0.0f);
			ChildMeshesStartAlpha.Add(0.0f);
		}
	}

	UE_LOG(LogTemp, Display, TEXT("Successfully created %d dynamic materials for child meshes"), ChildMeshesDynamicMaterials.Num());
}

void ASLBossCharacter::SetAllMeshesAlpha(float AlphaValue)
{
	float ClampedAlpha = FMath::Clamp(AlphaValue, 0.0f, 1.0f);
	
	for (int32 i = 0; i < ChildMeshesDynamicMaterials.Num(); i++)
	{
		if (ChildMeshesDynamicMaterials[i])
		{
			ChildMeshesDynamicMaterials[i]->SetScalarParameterValue(FName("Alpha"), ClampedAlpha);
			ChildMeshesCurrentAlpha[i] = ClampedAlpha;
		}
	}
}

void ASLBossCharacter::FadeAllMeshes(float InTargetAlpha, float Duration)
{
	if (ChildMeshesDynamicMaterials.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("FadeAllMeshes: No dynamic materials created"));
		return;
	}

	// 현재 Alpha 값들을 시작값으로 저장
	for (int32 i = 0; i < ChildMeshesCurrentAlpha.Num(); i++)
	{
		ChildMeshesStartAlpha[i] = ChildMeshesCurrentAlpha[i];
	}

	AllMeshesTargetAlpha = FMath::Clamp(InTargetAlpha, 0.0f, 1.0f);
	AllMeshesFadeDuration = FMath::Max(Duration, 0.01f);
	AllMeshesFadeStartTime = GetWorld()->GetTimeSeconds();

	// 기존 타이머 정리
	if (AllMeshesFadeTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(AllMeshesFadeTimerHandle);
	}

	// 새 타이머 시작
	GetWorld()->GetTimerManager().SetTimer(
		AllMeshesFadeTimerHandle,
		this,
		&ASLBossCharacter::UpdateAllMeshesFade,
		0.016f,
		true
	);
}

void ASLBossCharacter::UpdateAllMeshesFade()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float ElapsedTime = CurrentTime - AllMeshesFadeStartTime;
	float FadeProgress = FMath::Clamp(ElapsedTime / AllMeshesFadeDuration, 0.0f, 1.0f);

	// 모든 메시에 대해 보간
	for (int32 i = 0; i < ChildMeshesDynamicMaterials.Num(); i++)
	{
		if (ChildMeshesDynamicMaterials[i] && i < ChildMeshesStartAlpha.Num())
		{
			float NewAlpha = FMath::Lerp(ChildMeshesStartAlpha[i], AllMeshesTargetAlpha, FadeProgress);
			ChildMeshesDynamicMaterials[i]->SetScalarParameterValue(FName("Alpha"), NewAlpha);
			ChildMeshesCurrentAlpha[i] = NewAlpha;
		}
	}

	// 완료 체크
	if (FadeProgress >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(AllMeshesFadeTimerHandle);
	}
}

void ASLBossCharacter::SetMeshAlphaByIndex(int32 MeshIndex, float AlphaValue)
{
	if (!ChildMeshesDynamicMaterials.IsValidIndex(MeshIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetMeshAlphaByIndex: Invalid mesh index %d"), MeshIndex);
		return;
	}

	if (ChildMeshesDynamicMaterials[MeshIndex])
	{
		float ClampedAlpha = FMath::Clamp(AlphaValue, 0.0f, 1.0f);
		ChildMeshesDynamicMaterials[MeshIndex]->SetScalarParameterValue(FName("Alpha"), ClampedAlpha);
		ChildMeshesCurrentAlpha[MeshIndex] = ClampedAlpha;
	}
}

void ASLBossCharacter::FadeMeshByIndex(int32 MeshIndex, float InTargetAlpha, float Duration)
{
	if (!ChildMeshesDynamicMaterials.IsValidIndex(MeshIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("FadeMeshByIndex: Invalid mesh index %d"), MeshIndex);
		return;
	}

	// 기존 타이머가 있으면 정리
	if (FTimerHandle* ExistingTimer = IndividualFadeTimers.Find(MeshIndex))
	{
		if (ExistingTimer->IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(*ExistingTimer);
		}
	}

	// 개별 타이머를 사용한 페이드
	float StartAlpha = ChildMeshesCurrentAlpha[MeshIndex];
	float TargetAlpha = FMath::Clamp(InTargetAlpha, 0.0f, 1.0f);
	float FadeDuration = FMath::Max(Duration, 0.01f);
	float StartTime = GetWorld()->GetTimeSeconds();

	FTimerDelegate TimerDelegate;
	FTimerHandle NewTimer;
	
	TimerDelegate.BindLambda([this, MeshIndex, StartAlpha, TargetAlpha, FadeDuration, StartTime]()
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		float ElapsedTime = CurrentTime - StartTime;
		float Progress = FMath::Clamp(ElapsedTime / FadeDuration, 0.0f, 1.0f);
		
		if (ChildMeshesDynamicMaterials.IsValidIndex(MeshIndex) && ChildMeshesDynamicMaterials[MeshIndex])
		{
			float NewAlpha = FMath::Lerp(StartAlpha, TargetAlpha, Progress);
			ChildMeshesDynamicMaterials[MeshIndex]->SetScalarParameterValue(FName("Alpha"), NewAlpha);
			ChildMeshesCurrentAlpha[MeshIndex] = NewAlpha;
		}
		
		if (Progress >= 1.0f)
		{
			// 타이머 정리
			if (FTimerHandle* Timer = IndividualFadeTimers.Find(MeshIndex))
			{
				GetWorld()->GetTimerManager().ClearTimer(*Timer);
				IndividualFadeTimers.Remove(MeshIndex);
			}
		}
	});

	GetWorld()->GetTimerManager().SetTimer(NewTimer, TimerDelegate, 0.016f, true);
	IndividualFadeTimers.Add(MeshIndex, NewTimer);
}
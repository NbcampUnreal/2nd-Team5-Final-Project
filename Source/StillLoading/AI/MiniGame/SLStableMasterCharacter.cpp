#include "SLStableMasterCharacter.h"

#include "AI/AIInterface/SLHideableInterface.h"
#include "Controller/SLStableMasterAIController.h"
#include "Components/BoxComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogStableMaster, Log, All);

ASLStableMasterCharacter::ASLStableMasterCharacter()
{
	// AI 컨트롤러 설정
	AIControllerClass = ASLStableMasterAIController::StaticClass();
	
	// 마구간 주인 기본 설정
	MaxHealth = 10000.0f;
	bIsInvincibility = true;
	SetHitReactionMode(EHitReactionMode::EHRM_Disabled);
	AIChapter = EChapter::EC_Chapter3;

	// 감지 범위 설정
	NormalDetectionRange = 100.0f;      // 일반 상태 감지 범위
	HidingDetectionRange = 300.0f;      // 숨어있는 상태 감지 범위 (더 넓게)
	
	StableMasterController = nullptr;
}

void ASLStableMasterCharacter::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 부모 클래스의 충돌 처리 먼저 호출
	Super::OnBodyCollisionBoxBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	// SLHideableInterface를 구현한 액터와의 충돌 처리
	if (OtherActor && OtherActor->GetClass()->ImplementsInterface(USLHideableInterface::StaticClass()))
	{
		HandleHideableCharacterCollision(OtherActor);
	}
}

void ASLStableMasterCharacter::OnBodyCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 범위를 벗어난 액터를 처리 목록에서 제거 (상태 변화 감지를 위함)
	if (OtherActor && OtherActor->GetClass()->ImplementsInterface(USLHideableInterface::StaticClass()))
	{
		ProcessedActors.Remove(OtherActor);
	}
}

void ASLStableMasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	StableMasterController = Cast<ASLStableMasterAIController>(GetController());
	
	// 숨어있는 플레이어 감지를 위해 충돌 박스 크기 확장
	if (BoxCollisionComponent)
	{
		BoxCollisionComponent->SetBoxExtent(FVector(150.0f, 150.0f, 120.0f));
		
		// EndOverlap 이벤트 바인딩 (상태 변화 감지용)
		BoxCollisionComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &ASLStableMasterCharacter::OnBodyCollisionBoxEndOverlap);
	}
}

void ASLStableMasterCharacter::HandleHideableCharacterCollision(AActor* HideableCharacter)
{
	if (!HideableCharacter)
	{
		return;
	}

	// 중복 이벤트 방지
	if (ProcessedActors.Contains(HideableCharacter))
	{
		return;
	}

	// 거리 체크
	float Distance = FVector::Dist(GetActorLocation(), HideableCharacter->GetActorLocation());

	// 현재 숨어있는 상태인지 확인
	bool bIsCurrentlyHiding = ISLHideableInterface::Execute_IsHiding(HideableCharacter);
	
	if (!bIsCurrentlyHiding)
	{
		// 숨고 있지 않은 상태에서의 충돌 처리
		if (Distance <= NormalDetectionRange)
		{
			UE_LOG(LogStableMaster, Warning, TEXT("Player detected (not hiding): %s"), *HideableCharacter->GetName());
			OnHideableCharacterCollided(HideableCharacter);
			ProcessedActors.Add(HideableCharacter);
		}
	}
	else
	{
		// 숨고 있는 상태에서의 충돌 처리
		if (Distance <= HidingDetectionRange)
		{
			bool bIsBeingWatched = ISLHideableInterface::Execute_IsBeingWatched(HideableCharacter);
			
			if (bIsBeingWatched)
			{
				UE_LOG(LogStableMaster, Warning, TEXT("Hidden player detected while spotted: %s"), *HideableCharacter->GetName());
				OnHidingCharacterCollidedWhileSpotted(HideableCharacter);
				ProcessedActors.Add(HideableCharacter);
			}
		}
	}
}
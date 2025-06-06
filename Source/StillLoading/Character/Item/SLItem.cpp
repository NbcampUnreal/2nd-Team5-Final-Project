#include "SLItem.h"

#include "Character/SLPlayerCharacter.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Character/DataAsset/AttackDataAsset.h"
#include "GameFramework/Character.h"

ASLItem::ASLItem()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(SceneComponent);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(SceneComponent);

	OrbitCenter = CreateDefaultSubobject<USceneComponent>(TEXT("OrbitCenter"));
	OrbitCenter->SetupAttachment(SceneComponent);
}

void ASLItem::BeginPlay()
{
	Super::BeginPlay();
}

void ASLItem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (bIsOrbiting && OrbitCenter)
	{
		// 공전
		OrbitAngle += DeltaSeconds * OrbitSpeed;
		OrbitRadius += DeltaSeconds * RadiusExpandSpeed;
		
		const float Radians = FMath::DegreesToRadians(OrbitAngle);
		const FVector Offset = FVector(FMath::Cos(Radians) * OrbitRadius, FMath::Sin(Radians) * OrbitRadius, 0.f);

		const FVector NewLocation = OrbitCenter->GetComponentLocation() + Offset;
		SetActorLocation(NewLocation);
		BeginSweep();

		if (ItemMesh)
		{
			// 자전
			FRotator CurrentRotation = ItemMesh->GetRelativeRotation();
			CurrentRotation.Yaw += DeltaSeconds * RotationSpeed;
			ItemMesh->AddRelativeRotation(FRotator(0.f, RotationSpeed * DeltaSeconds, 0.f));
		}

		if (OrbitAngle >= 1080.f)
		{
			EndOrbit();
		}

		LastLocation = GetActorLocation();
	}
}

void ASLItem::BindOverlap(UPrimitiveComponent* OverlapComponent)
{
	if (OverlapComponent) {
		// 오버랩 연결시에 사용, 지금은 스윕으로 사용
		//OverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &ASLItem::HandleOverlap);
	}
}

void ASLItem::BeginSweep()
{
	if (ItemMesh)
	{
		const FVector StartLocation = LastLocation;
		const FVector EndLocation = GetActorLocation();

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		
		if (OwningCharacter)
		{
			QueryParams.AddIgnoredActor(OwningCharacter);
		}

		TArray<FHitResult> HitResults;
		
		const bool bHit = GetWorld()->SweepMultiByChannel(
			HitResults,
			StartLocation,
			EndLocation,
			FQuat::Identity,
			ECC_GameTraceChannel1,
			FCollisionShape::MakeBox(ItemMesh->Bounds.BoxExtent),
			QueryParams
		);

		//const FColor LineColor = bHit ? FColor::Red : FColor::Green;
		//DrawDebugBox(GetWorld(), EndLocation, ItemMesh->Bounds.BoxExtent, LineColor, false, 0.1f, 0, 2.f);

		if (bHit)
		{
			for (const FHitResult& Hit : HitResults)
			{
				AActor* HitActor = Hit.GetActor();
				if (HitActor && !HitActors.Contains(HitActor))
				{
					if (UBattleComponent* TargetBattleComp = HitActor->FindComponentByClass<UBattleComponent>())
					{
						HitActors.Add(HitActor);
						TargetBattleComp->SendHitResult(HitActor, Hit, EAttackAnimType::AAT_SpecialAttack1);
					}
				}
			}
		}
	}
}

void ASLItem::StartOrbit(ACharacter* InOwner)
{
	OwningCharacter = InOwner;

	ItemMesh->SetRelativeRotation(FRotator(90.f, 180.f, -90.f));

	if (OrbitCenter && InOwner)
	{
		OrbitCenter->AttachToComponent(InOwner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		OrbitCenter->SetRelativeLocation(FVector(0.f, 0.f, 20.f));
	}

	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ItemMesh->SetCollisionObjectType(ECC_WorldDynamic);
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);

	bIsOrbiting = true;
	OrbitAngle = 0.f;
	SetActorTickEnabled(true);
}

void ASLItem::EndOrbit()
{
	bIsOrbiting = false;
	SetActorTickEnabled(false);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			
	if (const ASLPlayerCharacter* PlayerCharacter = Cast<ASLPlayerCharacter>(OwningCharacter))
	{
		if (PlayerCharacter->Sword)
		{
			PlayerCharacter->Sword->SetActorHiddenInGame(false);
		}
	}
			
	Destroy();
}

void ASLItem::HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner() || HitActors.Contains(OtherActor)) return;

	UE_LOG(LogTemp, Log, TEXT("[%s] Overlapped with: %s"), *GetName(), *OtherActor->GetName());
	
	HitActors.Add(OtherActor);
	OnItemOverlap.Broadcast(OtherActor);
}


#include "FallingSword.h"

#include "NiagaraFunctionLibrary.h"
#include "AI/RealAI/MonsterAICharacter.h"
#include "Character/SLAIBaseCharacter.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Character/SlowMotionHelper/SlowMotionHelper.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AFallingSword::AFallingSword()
{
	PrimaryActorTick.bCanEverTick = true;

	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordMesh"));
	RootComponent = SwordMesh;
	SwordMesh->SetCollisionProfileName(TEXT("NoCollision"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 500.f;
	ProjectileMovement->MaxSpeed = 10000.f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->ProjectileGravityScale = 5.0f;
	ProjectileMovement->SetUpdatedComponent(RootComponent);
}

void AFallingSword::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(MaxLifetime);

	if (TrailEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(TrailEffect, RootComponent, NAME_None, FVector::ZeroVector,
		                                             FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
	}

	if (BeginSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BeginSound, GetActorLocation());
	}

	GetWorld()->GetTimerManager().SetTimer(
		DamageTimerHandle,
		this,
		&AFallingSword::ApplySweepDamage,
		DamageInterval,
		true);

	AActor* DamageCauser = GetInstigator(); 
	if (!DamageCauser)
	{
		DamageCauser = this; 
		UE_LOG(LogTemp, Warning, TEXT("FallingSword has no Instigator. Falling back to self."));
	}
	TargetActor = DamageCauser;
	USlowMotionHelper::ApplySlowMotionToPawns(DamageCauser, 0.2f, 3.0f);
}

void AFallingSword::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bImpactTriggered || !TargetActor.IsValid())
	{
		return;
	}

	if (GetActorLocation().Z + 300.f <= TargetActor->GetActorLocation().Z)
	{
		bImpactTriggered = true;
        
		TriggerImpact();

		SetActorTickEnabled(false);
	}
}

void AFallingSword::TriggerImpact()
{
	GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandle);

	ApplySweepDamage();

	if (ImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactEffect, GetActorLocation(),
													   GetActorRotation());
	}
	
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
	}

	ProjectileMovement->StopMovementImmediately();
	
	SwordMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SwordMesh->SetSimulatePhysics(false);

	SetLifeSpan(2.0f);
}

void AFallingSword::ApplySweepDamage()
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	AActor* DamageCauser = GetInstigator(); 
	if (!DamageCauser)
	{
		DamageCauser = this; 
		UE_LOG(LogTemp, Warning, TEXT("FallingSword has no Instigator. Falling back to self."));
	}

	TArray<FHitResult> HitResults;
	UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation(),
		DamageRadius,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResults,
		true);

	for (const FHitResult& Hit : HitResults)
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			if (HitActor->IsA(AMonsterAICharacter::StaticClass()) || HitActor->IsA(ASLAIBaseCharacter::StaticClass()))
			{
				if (UBattleComponent* BattleComp = HitActor->FindComponentByClass<UBattleComponent>())
				{
					BattleComp->ReceiveHitResult(5, DamageCauser, Hit, EAttackAnimType::AAT_SpecialAttack3);
				}
			}
		}
	}
}


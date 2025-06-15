#include "FallingSword.h"

#include "NiagaraFunctionLibrary.h"
#include "AI/RealAI/MonsterAICharacter.h"
#include "Character/SLAIBaseCharacter.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AFallingSword::AFallingSword()
{
	PrimaryActorTick.bCanEverTick = false;

	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordMesh"));
	RootComponent = SwordMesh;
	SwordMesh->SetCollisionProfileName(TEXT("NoCollision"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 50.f;
	ProjectileMovement->MaxSpeed = 2000.f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->ProjectileGravityScale = 1.0f;
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

	FScriptDelegate StopDelegate;
	StopDelegate.BindUFunction(this, "OnSwordImpact");
	ProjectileMovement->OnProjectileStop.Add(StopDelegate);

	GetWorld()->GetTimerManager().SetTimer(
		DamageTimerHandle,
		this,
		&AFallingSword::ApplySweepDamage,
		DamageInterval,
		true);
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
					BattleComp->ReceiveHitResult(50, DamageCauser, Hit, EAttackAnimType::AAT_SpecialAttack3);
				}
			}
		}
	}
}

void AFallingSword::OnSwordImpact(const FHitResult& HitResult)
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

	SetLifeSpan(3.0f);
}

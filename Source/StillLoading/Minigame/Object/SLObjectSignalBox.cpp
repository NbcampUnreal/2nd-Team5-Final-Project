// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLObjectSignalBox.h"

#include "Kismet/GameplayStatics.h"
#include "Misc/OutputDeviceNull.h"

// Sets default values
ASLObjectSignalBox::ASLObjectSignalBox()
{

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);


	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASLObjectSignalBox::BeginOverlap);

	bEndOverlapDeActivate = true;

}

// Called when the game starts or when spawned
void ASLObjectSignalBox::BeginPlay()
{
	Super::BeginPlay();

}

void ASLObjectSignalBox::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap"));
	if (!TargetActors.IsEmpty())
	{
		GetWorldTimerManager().SetTimer(DeactivateHandler, this, &ASLObjectSignalBox::DeactivateTrapFloor, VisibleTime, false);
		if (!bIsActive)
		{
			FVector MyLocation = GetActorLocation();
			MyLocation.Z -= 12;
			SetActorLocation(MyLocation);
			bIsActive = true;
			UGameplayStatics::PlaySound2D(this, BeginOverlapSound, 1.0f);
		}
		for (AActor* TargetActor : TargetActors)
		{
			if (TargetActor && EnterFunctionName != NAME_None)
			{
				
				FOutputDeviceNull Ar;
				TargetActor->CallFunctionByNameWithArguments(*EnterFunctionName.ToString(), Ar, nullptr, true);
				UE_LOG(LogTemp, Warning, TEXT("call FUNC"));

			}

		}
	}

}

void ASLObjectSignalBox::DeactivateTrapFloor()
{
	if (!TargetActors.IsEmpty())
	{
		UGameplayStatics::PlaySound2D(this, EndOverlapSound, 1.0f);
		FVector MyLocation = GetActorLocation();
		MyLocation.Z += 12;
		SetActorLocation(MyLocation);
		bIsActive = false;
		for (AActor* TargetActor : TargetActors)
		{
			if (TargetActor && EnterFunctionName != NAME_None)
			{
				FOutputDeviceNull Ar;
				TargetActor->CallFunctionByNameWithArguments(*ExitFunctionName.ToString(), Ar, nullptr, true);
			}
		}
	}
}



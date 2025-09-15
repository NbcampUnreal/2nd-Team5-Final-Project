// SLRunnerHurdle.cpp
#include "Minigame/Object/SLRunnerHurdle.h"
#include "Character/SLPlayerRunnerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"

ASLRunnerHurdle::ASLRunnerHurdle()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneComp      = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
    SetRootComponent(SceneComp);

    StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
    StaticMeshComp->SetupAttachment(SceneComp);
    StaticMeshComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    MainBoxComp    = CreateDefaultSubobject<UBoxComponent>(TEXT("MainBoxComp"));
    MainBoxComp->SetupAttachment(SceneComp);

    WarningBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("WarningBoxComp"));
    WarningBoxComp->SetupAttachment(SceneComp);

    Tags.Add(TEXT("Hurdle"));
}

void ASLRunnerHurdle::BeginPlay()
{
    Super::BeginPlay();

    if (WarningBoxComp)
    {
        WarningBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ASLRunnerHurdle::OnWarningOverlap);
    }
    if (MainBoxComp)
    {
        MainBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ASLRunnerHurdle::OnMainOverlap);
    }
    if (bRandomizeKey)
    {
        RandomizeExpectedInputKey();
    }
}

void ASLRunnerHurdle::RandomizeExpectedInputKey()
{
    if (CandidateKeys.Num() <= 0) { ExpectedInputKey = TEXT("W"); return; }
    const int32 Index = FMath::RandRange(0, CandidateKeys.Num() - 1);
    ExpectedInputKey = CandidateKeys[Index];
    OnQTEKeyChosen.Broadcast(ExpectedInputKey);
}

void ASLRunnerHurdle::OnWarningOverlap(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIdx, bool bFromSweep, const FHitResult& Hit)
{
    ASLPlayerRunnerCharacter* Player = Cast<ASLPlayerRunnerCharacter>(Other);
    if (!Player || HurdleState == EHurdleState::None) return;
    
    UGameplayStatics::SetGlobalTimeDilation(this, 0.15f);
    UGameplayStatics::SetGlobalPitchModulation(this, 0.15f, 0.f);

    WindowSec = ComputeWindowSeconds(Player);
    Player->QTE_Begin(ExpectedInputKey, WindowSec, HurdleState, RunnerMontageSection, FailMontageSection);
    CreateWidget();

    WarningBoxComp->SetGenerateOverlapEvents(false);
}

void ASLRunnerHurdle::OnMainOverlap(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIdx, bool bFromSweep, const FHitResult& Hit)
{
    if (!Other->IsA<ASLPlayerRunnerCharacter>()) return;
    if (!bIsUsed) return; 
    MainBoxComp->SetGenerateOverlapEvents(false);
    SetDefaultHurdleState();
}

float ASLRunnerHurdle::ComputeWindowSeconds(ASLPlayerRunnerCharacter* Player) const
{
    if (!Player || !WarningBoxComp || !MainBoxComp) return 0.f;

    USplineComponent* Spline = Player->GetTrackSpline();
    if (!Spline) return 0.f;

    const float StartDist = Spline->GetDistanceAlongSplineAtSplineInputKey(
        Spline->FindInputKeyClosestToWorldLocation(WarningBoxComp->GetComponentLocation()));

    const float TargetDist = Spline->GetDistanceAlongSplineAtSplineInputKey(
        Spline->FindInputKeyClosestToWorldLocation(MainBoxComp->GetComponentLocation()));

    float PathDist = FMath::Max(0.f, TargetDist - StartDist);

    const float PlayerHalf = Player->FindComponentByClass<UBoxComponent>()
        ? Player->FindComponentByClass<UBoxComponent>()->GetScaledBoxExtent().X : 50.f;

    const float BoxHalfAlongPath = MainBoxComp->GetScaledBoxExtent().X;
    const float OverlapMargin = PlayerHalf + BoxHalfAlongPath;

    PathDist = FMath::Max(0.f, PathDist - OverlapMargin);

    const float Speed = FMath::Max(1.f, Player->ForwardSpeed * 0.15f); // 타임 슬로우 반영
    return PathDist / Speed;
}

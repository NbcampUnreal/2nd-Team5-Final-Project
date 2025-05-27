#include "SLGridVolumeManager.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

ASLGridVolumeManager::ASLGridVolumeManager()
{
    PrimaryActorTick.bCanEverTick = true;
    
#if WITH_EDITOR
    PrimaryActorTick.bStartWithTickEnabled = true;
#else
    PrimaryActorTick.bStartWithTickEnabled = false;
#endif
    
    // 기본 설정
    GridCountX = 5;
    GridCountY = 5;
    GridWidth = 800.0f;
    GridHeight = 600.0f;
    CameraFitch = 30.0f;
    
    InitializeGridArray();
}

void ASLGridVolumeManager::BeginPlay()
{
    Super::BeginPlay();
    
    RefreshAllGridVolumes();
}

void ASLGridVolumeManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
#if WITH_EDITOR
    // 그리드 설정이 변경되었는지 확인
    if (HasGridSettingsChanged())
    {
        InitializeGridArray();
        UpdateExistingVolumes();
        UpdateLastGridSettings();
    }
#endif
}

#if WITH_EDITOR
void ASLGridVolumeManager::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    
    InitializeGridArray();
    RefreshAllGridVolumes();
}

void ASLGridVolumeManager::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    
    if (PropertyChangedEvent.Property)
    {
        FName PropertyName = PropertyChangedEvent.Property->GetFName();
        
        // 그리드 설정이 변경된 경우
        if (PropertyName == GET_MEMBER_NAME_CHECKED(ASLGridVolumeManager, GridCountX) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(ASLGridVolumeManager, GridCountY) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(ASLGridVolumeManager, GridWidth) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(ASLGridVolumeManager, GridHeight))
        {
            InitializeGridArray();
            UpdateExistingVolumes();
        }
        
        // 자동 생성 옵션이 켜진 경우
        if (PropertyName == GET_MEMBER_NAME_CHECKED(ASLGridVolumeManager, bAutoGenerateVolumes) && bAutoGenerateVolumes)
        {
            GenerateGridVolumes();
            bAutoGenerateVolumes = false; // 한 번만 실행되도록
        }
    }
}
#endif

ASLGridVolume* ASLGridVolumeManager::GetGridVolume(int32 X, int32 Y) const
{
    if (X < 0 || X >= GridCountX || Y < 0 || Y >= GridCountY)
    {
        return nullptr;
    }
    
    if (GridVolumes.IsValidIndex(X) && GridVolumes[X].IsValidIndex(Y))
    {
        return GridVolumes[X][Y];
    }
    
    return nullptr;
}

ASLGridVolume* ASLGridVolumeManager::GetGridVolumeAtWorldPosition(const FVector& WorldPosition) const
{
    FIntPoint GridCoord = GetGridCoordinateFromWorldPosition(WorldPosition);
    return GetGridVolume(GridCoord.X, GridCoord.Y);
}

FVector ASLGridVolumeManager::GetWorldPositionFromGridCoordinate(int32 X, int32 Y) const
{
    FVector ManagerLocation = GetActorLocation();
    
    float CellWidth = GetCellWidth();
    float CellHeight = GetCellHeight();
    
    // 카메라 피치에 따른 세로 스케일링은 이미 GetCellHeight()에서 처리됨
    
    // 정확히 그리드 매니저가 중앙에 오도록 계산
    // GridCountX와 GridCountY가 홀수인지 짝수인지에 따라 오프셋 조정
    float OffsetX = (GridCountX % 2 == 0) ? CellWidth * 0.5f : 0.0f;
    float OffsetY = (GridCountY % 2 == 0) ? CellHeight * 0.5f : 0.0f;
    
    // 그리드의 시작점 계산 (그리드 매니저가 중앙에 오도록)
    float WorldX = ManagerLocation.X + ((X - (GridCountX / 2)) * CellWidth) - OffsetX;
    float WorldY = ManagerLocation.Y + ((Y - (GridCountY / 2)) * CellHeight) - OffsetY;
    
    return FVector(WorldX, WorldY, ManagerLocation.Z + GroundHeight);
}

FIntPoint ASLGridVolumeManager::GetGridCoordinateFromWorldPosition(const FVector& WorldPosition) const
{
    FVector ManagerLocation = GetActorLocation();
    
    float CellWidth = GetCellWidth();
    float CellHeight = GetCellHeight();
    
    // 정확히 그리드 매니저가 중앙에 오도록 계산
    // GridCountX와 GridCountY가 홀수인지 짝수인지에 따라 오프셋 조정
    float OffsetX = (GridCountX % 2 == 0) ? CellWidth * 0.5f : 0.0f;
    float OffsetY = (GridCountY % 2 == 0) ? CellHeight * 0.5f : 0.0f;
    
    // 월드 위치에서 그리드 좌표 계산
    int32 GridX = FMath::FloorToInt((WorldPosition.X - ManagerLocation.X + OffsetX) / CellWidth + (GridCountX / 2));
    int32 GridY = FMath::FloorToInt((WorldPosition.Y - ManagerLocation.Y + OffsetY) / CellHeight + (GridCountY / 2));
    
    return FIntPoint(GridX, GridY);
}

void ASLGridVolumeManager::RegisterGridVolume(ASLGridVolume* Volume, int32 X, int32 Y)
{
    if (!Volume || X < 0 || X >= GridCountX || Y < 0 || Y >= GridCountY)
    {
        return;
    }
    
    if (GridVolumes.IsValidIndex(X) && GridVolumes[X].IsValidIndex(Y))
    {
        GridVolumes[X][Y] = Volume;
        // Volume->SetGridManager(this);
        // Volume->SetGridCoordinate(FIntPoint(X, Y));
        
        // 월드 위치 설정
        FVector TargetLocation = GetWorldPositionFromGridCoordinate(X, Y);
        Volume->SetActorLocation(TargetLocation);
    }
}

void ASLGridVolumeManager::UnregisterGridVolume(ASLGridVolume* Volume)
{
    if (!Volume) return;
    
    for (int32 X = 0; X < GridVolumes.Num(); ++X)
    {
        for (int32 Y = 0; Y < GridVolumes[X].Num(); ++Y)
        {
            if (GridVolumes[X][Y] == Volume)
            {
                GridVolumes[X][Y] = nullptr;
                return;
            }
        }
    }
}

void ASLGridVolumeManager::RefreshAllGridVolumes()
{
    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASLGridVolume::StaticClass(), FoundVolumes);
    
    // 기존 등록 정보 초기화
    InitializeGridArray();
    
    for (AActor* Actor : FoundVolumes)
    {
        ASLGridVolume* Volume = Cast<ASLGridVolume>(Actor);
        if (!Volume) continue;
        
        // 월드 위치를 기반으로 그리드 좌표 계산
        FIntPoint GridCoord = GetGridCoordinateFromWorldPosition(Volume->GetActorLocation());
        
        // 유효한 범위 내의 볼륨만 등록
        if (GridCoord.X >= 0 && GridCoord.X < GridCountX && 
            GridCoord.Y >= 0 && GridCoord.Y < GridCountY)
        {
            RegisterGridVolume(Volume, GridCoord.X, GridCoord.Y);
        }
    }
}

void ASLGridVolumeManager::GenerateGridVolumes()
{
#if WITH_EDITOR
    UWorld* World = GetWorld();
    if (!World) return;
    
    for (int32 X = 0; X < GridCountX; ++X)
    {
        for (int32 Y = 0; Y < GridCountY; ++Y)
        {
            // 이미 볼륨이 있는 위치는 건너뛰기
            if (GetGridVolume(X, Y) != nullptr) continue;
            
            FVector SpawnLocation = GetWorldPositionFromGridCoordinate(X, Y);
            FRotator SpawnRotation = FRotator::ZeroRotator;
            
            ASLGridVolume* NewVolume = World->SpawnActor<ASLGridVolume>(ASLGridVolume::StaticClass(), 
                                                                       SpawnLocation, SpawnRotation);
            if (NewVolume)
            {
                RegisterGridVolume(NewVolume, X, Y);
                
                // 액터 이름 설정
                FString ActorName = FString::Printf(TEXT("GridVolume_%d_%d"), X, Y);
                NewVolume->SetActorLabel(ActorName);
            }
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("Generated %d x %d grid volumes"), GridCountX, GridCountY);
#endif
}

void ASLGridVolumeManager::ClearAllGridVolumes()
{
#if WITH_EDITOR
    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASLGridVolume::StaticClass(), FoundVolumes);
    
    for (AActor* Actor : FoundVolumes)
    {
        if (Actor)
        {
            Actor->Destroy();
        }
    }
    
    InitializeGridArray();
    UE_LOG(LogTemp, Log, TEXT("Cleared all grid volumes"));
#endif
}

float ASLGridVolumeManager::GetCellWidth() const
{
    const float PitchRadians = FMath::DegreesToRadians(CameraFitch);
    const float HeightScale = 1.f / FMath::Cos(PitchRadians);
    return GridHeight * HeightScale;
}

float ASLGridVolumeManager::GetCellHeight() const
{
    return GridWidth;
}

void ASLGridVolumeManager::InitializeGridArray()
{
    GridVolumes.Empty();
    GridVolumes.SetNum(GridCountX);
    
    for (int32 X = 0; X < GridCountX; ++X)
    {
        GridVolumes[X].SetNum(GridCountY);
        for (int32 Y = 0; Y < GridCountY; ++Y)
        {
            GridVolumes[X][Y] = nullptr;
        }
    }
}


void ASLGridVolumeManager::UpdateExistingVolumes()
{
    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASLGridVolume::StaticClass(), FoundVolumes);
    
    for (AActor* Actor : FoundVolumes)
    {
        ASLGridVolume* Volume = Cast<ASLGridVolume>(Actor);
        if (!Volume) continue;
        
        // Volume->SetGridManager(this);
        
        // 현재 위치를 기반으로 그리드 좌표 재계산
        FIntPoint NewGridCoord = GetGridCoordinateFromWorldPosition(Volume->GetActorLocation());
        
        // 유효한 범위 내의 볼륨만 업데이트
        if (NewGridCoord.X >= 0 && NewGridCoord.X < GridCountX && 
            NewGridCoord.Y >= 0 && NewGridCoord.Y < GridCountY)
        {
            // Volume->SetGridCoordinate(NewGridCoord);
            if (GridVolumes.IsValidIndex(NewGridCoord.X) && GridVolumes[NewGridCoord.X].IsValidIndex(NewGridCoord.Y))
            {
                GridVolumes[NewGridCoord.X][NewGridCoord.Y] = Volume;
            }
        }
    }
}

bool ASLGridVolumeManager::HasGridSettingsChanged() const
{
    return (LastGridCountX != GridCountX || 
            LastGridCountY != GridCountY ||
            !FMath::IsNearlyEqual(LastGridWidth, GridWidth, 0.1f) ||
            !FMath::IsNearlyEqual(LastGridHeight, GridHeight, 0.1f));
}

void ASLGridVolumeManager::UpdateLastGridSettings()
{
    LastGridCountX = GridCountX;
    LastGridCountY = GridCountY;
    LastGridWidth = GridWidth;
    LastGridHeight = GridHeight;
}
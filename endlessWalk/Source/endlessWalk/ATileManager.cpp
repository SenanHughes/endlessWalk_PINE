// Fill out your copyright notice in the Description page of Project Settings.


#include "ATileManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATileManager::ATileManager()
{
    PrimaryActorTick.bCanEverTick = true;
    SpawnThreshold = 1000.0f;  // Adjust according to your tile size
    LastSpawnLocation = FVector::ZeroVector;
}

// Called when the game starts or when spawned
void ATileManager::BeginPlay()
{
    Super::BeginPlay();

    // Get the player actor (assumes player pawn is present)
    PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    // Find the WaterBodyRiver in the scene (you can also set it in the editor)
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaterBodyRiver::StaticClass(), FoundActors);

    if (FoundActors.Num() > 0)
    {
        WaterBodyRiver = Cast<AWaterBodyRiver>(FoundActors[0]);
        
        // Get the WaterBodyRiverComponent
        UWaterBodyRiverComponent* RiverComponent = Cast<UWaterBodyRiverComponent>(WaterBodyRiver->GetWaterBodyComponent());
        if (RiverComponent)
        {
            // Set WaterBodyRiverComponent to Movable
            RiverComponent->SetMobility(EComponentMobility::Movable);

            // Get and update the Water Spline Component
            UWaterSplineComponent* RiverSpline = RiverComponent->GetWaterSpline();
            if (RiverSpline)
            {
                RiverSpline->SetMobility(EComponentMobility::Movable);
            }
        }
    }

    // Spawn the initial tile at our starting location
    if (TileBlueprint)
    {
        LastSpawnLocation = GetActorLocation();
        SpawnTile(LastSpawnLocation);
    }
}

// Called every frame
void ATileManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PlayerActor)
    {
        FVector PlayerLocation = PlayerActor->GetActorLocation();

        // Check if the player has moved beyond the threshold from the last spawn location
        if (FVector::Dist(PlayerLocation, LastSpawnLocation) > SpawnThreshold)
        {
            // Calculate a new spawn location ahead of the player
            FVector Direction = (PlayerLocation - LastSpawnLocation).GetSafeNormal();
            FVector NewSpawnLocation = LastSpawnLocation + Direction * SpawnThreshold;

            SpawnTile(NewSpawnLocation);
            LastSpawnLocation = NewSpawnLocation;
        }
    }
}

void ATileManager::SpawnTile(const FVector& SpawnLocation)
{
    if (TileBlueprint)
    {
        // Spawn the tile at the given location with no rotation
        FActorSpawnParameters SpawnParams;
        GetWorld()->SpawnActor<AActor>(TileBlueprint, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

    }
     
    if (WaterBodyRiver)
    {
        UWaterBodyRiverComponent* RiverComponent = Cast<UWaterBodyRiverComponent>(WaterBodyRiver->GetWaterBodyComponent());
        if (RiverComponent)
        {
            UWaterSplineComponent* RiverSpline = RiverComponent->GetWaterSpline();
            if (RiverSpline)
            {
                // Get the position of the last spline point
                int32 LastSplinePointIndex = RiverSpline->GetNumberOfSplinePoints() - 1;
                FVector LastSplinePoint = RiverSpline->GetLocationAtSplinePoint(LastSplinePointIndex, ESplineCoordinateSpace::World);

                // Calculate the new spawn location by adding a desired offset (e.g., 300 units)
                FVector NewSpawnLocation = LastSplinePoint;
                NewSpawnLocation.Y += 1000.0f; // Adjust based on your logic

                // Log the positions for debugging
                UE_LOG(LogTemp, Warning, TEXT("Last Spline Point: %s"), *LastSplinePoint.ToString());
                UE_LOG(LogTemp, Warning, TEXT("New Spawn Location: %s"), *NewSpawnLocation.ToString());

                // Adjust the spline point with the new location
                AdjustSplinePoint(LastSplinePointIndex, NewSpawnLocation);

                // Draw spline points for debugging
                const int32 NumPoints = RiverSpline->GetNumberOfSplinePoints();
                for (int32 i = 0; i < NumPoints; ++i)
                {
                    FVector PointLocation = RiverSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
                    // Visualize the spline points using debug spheres
                    DrawDebugSphere(GetWorld(), PointLocation, 50.0f, 12, FColor::Green, false, 10.0f, 0, 1.0f);
                }

                // Ensure the landscape carving and visual updates are applied
                RiverComponent->MarkRenderStateDirty();
                WaterBodyRiver->ReregisterAllComponents();
            }
        }
    }
}

void ATileManager::AdjustSplinePoint(int32 PointIndex, FVector NewLocation)
{
    if (WaterBodyRiver)
    {
        UWaterBodyRiverComponent* RiverComponent = Cast<UWaterBodyRiverComponent>(WaterBodyRiver->GetWaterBodyComponent());
        if (RiverComponent)
        {
            UWaterSplineComponent* RiverSpline = RiverComponent->GetWaterSpline();
            if (RiverSpline)
            {
                // Ensure the point index is valid
                if (PointIndex >= 0 && PointIndex < RiverSpline->GetNumberOfSplinePoints())
                {
                    // Modify the position of the specified spline point
                    RiverSpline->SetLocationAtSplinePoint(PointIndex, NewLocation, ESplineCoordinateSpace::World);

                    // Apply the changes
                    RiverSpline->UpdateSpline();  // Update the spline to apply changes
                    RiverSpline->MarkPackageDirty();  // Mark it dirty so changes persist
                    RiverComponent->MarkRenderStateDirty();  // Refresh the rendering state

                    // Optional: Visualize the new position for debugging
                    DrawDebugSphere(GetWorld(), NewLocation, 50.0f, 12, FColor::Red, false, 10.0f, 0, 1.0f);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Invalid spline point index."));
                }
            }
        }
    }
}
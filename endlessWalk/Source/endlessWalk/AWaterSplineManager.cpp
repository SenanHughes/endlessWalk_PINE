// Fill out your copyright notice in the Description page of Project Settings.


#include "AWaterSplineManager.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

// Sets default values
AWaterSplineManager::AWaterSplineManager()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create and attach the spline component
    WaterSpline = CreateDefaultSubobject<USplineComponent>(TEXT("WaterSpline"));
    RootComponent = WaterSpline;

    SplineSpawnThreshold = 1000.0f;
    LastSplinePoint = FVector::ZeroVector;
}

// Called when the game starts or when spawned
void AWaterSplineManager::BeginPlay()
{
    Super::BeginPlay();

    PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    // Find the WaterBodyRiver in the scene
    for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        WaterBodyRiver = *ActorItr;
        if (WaterBodyRiver)
        {
            UE_LOG(LogTemp, Warning, TEXT("Found WaterBodyRiver by Class: %s"), *WaterBodyRiver->GetName());
            break;  // If we find one, stop searching
        }
    }

    if (!WaterBodyRiver)
    {
        UE_LOG(LogTemp, Error, TEXT("ERROR: WaterBodyRiver NOT FOUND by Class!"));
    }

    // Optionally, initialize the spline with the current location
    LastSplinePoint = GetActorLocation();
    WaterSpline->AddSplinePoint(LastSplinePoint, ESplineCoordinateSpace::World, true);
}

// Called every frame
void AWaterSplineManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PlayerActor)
    {
        FVector PlayerLocation = PlayerActor->GetActorLocation();

        // Add a new spline point if the player moves beyond the threshold
        if (FVector::Dist(PlayerLocation, LastSplinePoint) > SplineSpawnThreshold)
        {
            FVector Direction = (PlayerLocation - LastSplinePoint).GetSafeNormal();
            FVector NewSplinePoint = LastSplinePoint + Direction * SplineSpawnThreshold;

            UpdateWaterSpline(NewSplinePoint);
            LastSplinePoint = NewSplinePoint;
        }
    }
}

void AWaterSplineManager::UpdateWaterSpline(const FVector& NewPoint)
{
    if (!WaterSpline || !WaterBodyRiver) return;

    // Add the new spline point for the procedural spline
    WaterSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::World, true);
    WaterSpline->UpdateSpline();

    // Get the WaterBodyRiver's spline component
    USplineComponent* RiverSpline = WaterBodyRiver->FindComponentByClass<USplineComponent>();
    if (RiverSpline)
    {
        // Add a new spline point to the WaterBodyRiver's spline
        RiverSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::World, true);
        RiverSpline->UpdateSpline();
    }
}


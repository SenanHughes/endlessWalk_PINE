// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "AWaterSplineManager.generated.h"

UCLASS()
class ENDLESSWALK_API AWaterSplineManager : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AWaterSplineManager();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Spline component for water body path
    UPROPERTY(VisibleAnywhere, Category = "Water Spline")
    USplineComponent* WaterSpline;

    // Blueprint reference for the WaterBodyRiver actor (optional, if you wish to spawn it)
    UPROPERTY(EditAnywhere, Category = "Water")
    TSubclassOf<AActor> WaterBodyBlueprint;

    // Reference to the Player Actor
    UPROPERTY()
    AActor* PlayerActor;

    // Update the spline with a new point
    void UpdateWaterSpline(const FVector& NewPoint);

    void DebugPrintSplinePoints();

    // Distance threshold to add a new spline point
    UPROPERTY(EditAnywhere, Category = "Water")
    float SplineSpawnThreshold;

    // Store last spline point added
    FVector LastSplinePoint;

	// Reference to the WaterBodyRiver actor
	UPROPERTY(EditAnywhere, Category = "Water")
	AActor* WaterBodyRiver;
};

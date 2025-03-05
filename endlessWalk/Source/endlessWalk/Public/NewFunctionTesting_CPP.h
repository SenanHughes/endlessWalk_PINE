// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "ProceduralMeshComponent.h"
#include "NewFunctionTesting_CPP.generated.h"

class USplineComponent;
class UProceduralMeshComponent;

UCLASS()
class ENDLESSWALK_API ANewFunctionTesting_CPP : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANewFunctionTesting_CPP();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain")
	USplineComponent* PathSpline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProceduralMesh")
	UProceduralMeshComponent* PathMesh;

	TArray<FVector> PathVertices;
	TArray<FVector2D> PathUVs;
	TArray<int32> PathTriangles;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GenerateProcMesh();

};

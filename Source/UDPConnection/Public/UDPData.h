// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "UDPData.generated.h"

USTRUCT(BlueprintType)
struct FBicycleData

{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
		float SteeringDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
		float PlayerAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
		float BicycleAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
		float PhysicsDelta;

	FBicycleData()
	{}
};
FORCEINLINE FArchive& operator<<(FArchive& Ar, FBicycleData& BicycleData)
{
	Ar << BicycleData.SteeringDistance;
	Ar << BicycleData.PlayerAngle;
	Ar << BicycleData.BicycleAngle;
	Ar << BicycleData.PhysicsDelta;

	return Ar;
}
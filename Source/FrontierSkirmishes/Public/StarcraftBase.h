// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "StarcraftBase.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class FRONTIERSKIRMISHES_API AStarcraftBase : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:

	AStarcraftBase();
	
private:
	
	// UPROPERTY(VisibleAnywhere, Category = "Camera")
	// TObjectPtr<USpringArmComponent> CameraBoom;
	//
	// UPROPERTY(VisibleAnywhere, Category = "Camera")
	// TObjectPtr<UCameraComponent> FollowCamera;

protected:

};

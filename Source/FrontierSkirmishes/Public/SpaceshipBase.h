// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h" 
#include "SpaceshipBase.generated.h"

UCLASS()
class FRONTIERSKIRMISHES_API ASpaceshipBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASpaceshipBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    // Root component - the main collision
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USceneComponent* ShipRoot;

    // Visual mesh of the ship
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* ShipMesh;

    // Camera boom (spring arm) - follows ship at distance
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USpringArmComponent* CameraBoom;

    // Camera component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UCameraComponent* Camera;

    // Input Mapping Context
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputMappingContext* InputMapping;

    // Input Actions
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* ThrustAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* PitchAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* YawAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* RollAction;

    // MOVEMENT PROPERTIES
    // How fast the ship accelerates forward
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float ThrustPower;

    // Maximum speed the ship can reach
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MaxSpeed;

    // How quickly ship slows down when not thrusting
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float Drag;

    // How fast the ship rotates (pitch - up/down)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float PitchSpeed;

    // How fast the ship rotates (yaw - left/right)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float YawSpeed;

    // How fast the ship rotates (roll - barrel roll)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float RollSpeed;

    // CURRENT MOVEMENT STATE

    // Current velocity of the ship
    FVector CurrentVelocity;

    // INPUT FUNCTIONS

    // INPUT FUNCTIONS
    void ThrustInput(const FInputActionValue& Value);
    void PitchInput(const FInputActionValue& Value);
    void YawInput(const FInputActionValue& Value);
    void RollInput(const FInputActionValue& Value);

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};

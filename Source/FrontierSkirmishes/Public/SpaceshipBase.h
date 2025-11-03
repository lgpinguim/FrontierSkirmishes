#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "SpaceshipBase.generated.h"

// Forward declarations
//class ULockOnComponent;
//class UWeaponSystemComponent;
//class UTargetLeadComponent;

UCLASS()
class FRONTIERSKIRMISHES_API ASpaceshipBase : public APawn
{
    GENERATED_BODY()

public:
    ASpaceshipBase();

protected:
    virtual void BeginPlay() override;

    // COMPONENTS
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USceneComponent* ShipRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* ShipMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UCameraComponent* Camera;

    //TODO: Other Components
    //UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    //ULockOnComponent* LockOnComponent;

    //UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    //UWeaponSystemComponent* WeaponSystem;

    //UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    //UTargetLeadComponent* TargetLeadComponent;

    // INPUT
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputMappingContext* InputMapping;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* MouseAimAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* ThrottleAction; 

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* RollAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* BoostDodgeAction;  // Quick directional boost

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* PrimaryWeaponAction;  // Machine guns

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* SecondaryWeaponAction;  // Missiles

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* CycleWeaponAction;  // Switch missile types

    // MOVEMENT PROPERTIES
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MaxSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MinSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float Acceleration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float Deceleration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float TurnResponsiveness;  // How quickly ship follows mouse

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MaxTurnRate;  // Limits how fast we can turn

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float RollRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MouseSensitivity;  // How sensitive mouse input is

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float AutoRollStrength;  // How much ship rolls into turns

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float AutoLevelSpeed;  // How fast ship returns to level flight

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MaxPitchAngle;  // Maximum pitch up/down in degrees

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MaxYawAngle;    // Maximum yaw left/right in degrees

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MaxRollAngle;    // Maximum roll angle in degrees (recommended: 60-90 degrees)

    // BOOST DODGE 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost Dodge")
    float BoostDodgeStrength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost Dodge")
    float BoostDodgeDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost Dodge")
    float BoostDodgeCooldown;

    // CURRENT STATE
    FVector2D MouseInput;
    float ThrottleInput;
    float ManualRollInput;

    FVector CurrentVelocity;
    float CurrentSpeed;

    // Boost Dodge state
    bool bIsBoostDodging;
    FVector BoostDodgeDirection;
    float BoostDodgeTimer;
    float BoostDodgeCooldownTimer;

    // INPUT FUNCTIONS
    void HandleMouseAim(const FInputActionValue& Value);
    void HandleThrottle(const FInputActionValue& Value);
    void HandleRoll(const FInputActionValue& Value);
    void HandleBoostDodge(const FInputActionValue& Value);
    void HandlePrimaryWeapon(const FInputActionValue& Value);
    void HandleSecondaryWeapon(const FInputActionValue& Value);
    void HandleCycleWeapon(const FInputActionValue& Value);

    // ADVANCED MOVEMENT FUNCTIONS
    void UpdateRotation(float DeltaTime);
    void UpdateVelocity(float DeltaTime);
    void UpdateBoostDodge(float DeltaTime);

    // Smooth rotation helper
    FRotator SmoothRotateTowards(const FRotator& Current, const FRotator& Target, float DeltaTime, float InterpSpeed);

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Getters for UI and other systems
    UFUNCTION(BlueprintPure, Category = "Spaceship")
    float GetCurrentThrottlePercent() const { return (CurrentSpeed - MinSpeed) / (MaxSpeed - MinSpeed); }

    UFUNCTION(BlueprintPure, Category = "Spaceship")
    bool CanBoostDodge() const { return BoostDodgeCooldownTimer <= 0.0f && !bIsBoostDodging; }
};
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
    class UInputAction* PitchAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* YawAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* ThrottleAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* RollAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* BoostDodgeAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* BrakeAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* PrimaryWeaponAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* SecondaryWeaponAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* CycleWeaponAction;

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
    float BrakeDeceleration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float TurnResponsiveness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MaxTurnRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float RollRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MouseSensitivity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float AutoRollStrength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float AutoLevelSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool bEnableAutoLevel;  // Toggle auto-leveling on/off

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MaxPitchAngle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MaxRollAngle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool bEnableRotationLimits;

    // CAMERA PROPERTIES
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraLagSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraLagMaxDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float BaseFOV;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float BoostFOV;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float FOVInterpSpeed;

    // BOOST DODGE 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost Dodge")
    float BoostDodgeStrength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost Dodge")
    float BoostDodgeDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost Dodge")
    float BoostDodgeCooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost Dodge")
    float BoostDodgeCameraShakeScale;

    // CURRENT STATE
    float PitchInput;
    float YawInput;
    float ThrottleInput;
    float ManualRollInput;
    bool bIsBraking;
    bool bManualRollActive = false;

    float CurrentSpeed;
    float CurrentFOV;

    // Boost Dodge state
    bool bIsBoostDodging;
    FVector BoostDodgeDirection;
    float BoostDodgeTimer;
    float BoostDodgeCooldownTimer;

    // INPUT FUNCTIONS
    void HandlePitch(const FInputActionValue& Value);
    void HandleYaw(const FInputActionValue& Value);
    void HandleThrottle(const FInputActionValue& Value);
    void HandleRoll(const FInputActionValue& Value);
    void HandleBoostDodge(const FInputActionValue& Value);
    void HandleBrake(const FInputActionValue& Value);
    void HandleBrakeReleased(const FInputActionValue& Value);
    void HandlePrimaryWeapon(const FInputActionValue& Value);
    void HandleSecondaryWeapon(const FInputActionValue& Value);
    void HandleCycleWeapon(const FInputActionValue& Value);

    // MOVEMENT FUNCTIONS
    void UpdateRotation(float DeltaTime);
    void UpdateVelocity(float DeltaTime);
    void UpdateBoostDodge(float DeltaTime);
    void UpdateCamera(float DeltaTime);

    // HELPER FUNCTIONS
    FRotator ClampRotation(const FRotator& Rotation) const;
    void HandleCollision(const FHitResult& Hit);

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Getters for UI and other systems
    UFUNCTION(BlueprintPure, Category = "Spaceship")
    float GetCurrentThrottlePercent() const { return (CurrentSpeed - MinSpeed) / (MaxSpeed - MinSpeed); }

    UFUNCTION(BlueprintPure, Category = "Spaceship")
    bool CanBoostDodge() const { return BoostDodgeCooldownTimer <= 0.0f && !bIsBoostDodging; }

    UFUNCTION(BlueprintPure, Category = "Spaceship")
    FVector GetCurrentVelocity() const { return GetActorForwardVector() * CurrentSpeed; }

    UFUNCTION(BlueprintPure, Category = "Spaceship")
    float GetCurrentSpeed() const { return CurrentSpeed; }

    UFUNCTION(BlueprintPure, Category = "Spaceship")
    bool IsBraking() const { return bIsBraking; }
};
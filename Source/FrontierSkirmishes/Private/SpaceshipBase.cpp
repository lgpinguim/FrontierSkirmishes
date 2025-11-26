#include "SpaceshipBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "EnhancedInputComponent.h"  
#include "EnhancedInputSubsystems.h" 
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
//#include "LockOnComponent.h"
//#include "WeaponSystemComponent.h"
//#include "TargetLeadComponent.h"

ASpaceshipBase::ASpaceshipBase()
{
    PrimaryActorTick.bCanEverTick = true;

    ShipRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ShipRoot"));
    RootComponent = ShipRoot;

    ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
    ShipMesh->SetupAttachment(ShipRoot);
    ShipMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ShipMesh->SetCollisionResponseToAllChannels(ECR_Block);

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(ShipRoot);
    CameraBoom->TargetArmLength = 800.0f;
    CameraBoom->bUsePawnControlRotation = false;
    CameraBoom->bInheritPitch = false;
    CameraBoom->bInheritYaw = false;
    CameraBoom->bInheritRoll = false;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 3.0f;
    CameraBoom->CameraLagMaxDistance = 50.0f;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(CameraBoom);

    // Create advanced system components
    //LockOnComponent = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOnComponent"));
    //WeaponSystem = CreateDefaultSubobject<UWeaponSystemComponent>(TEXT("WeaponSystem"));
    //TargetLeadComponent = CreateDefaultSubobject<UTargetLeadComponent>(TEXT("TargetLeadComponent"));

    // Movement properties
    MaxSpeed = 4000.0f;
    MinSpeed = 0.0f;
    Acceleration = 1500.0f;
    Deceleration = 1200.0f;
    BrakeDeceleration = 3000.0f;

    MaxTurnRate = 240.0f;
    TurnResponsiveness = 3.0f;
    RollRate = 120.0f;

    MouseSensitivity = 1.0f;
    AutoRollStrength = 60.0f;
    AutoLevelSpeed = 2.0f;
    bEnableAutoLevel = false;  // Disabled by default for true space flight
    MaxPitchAngle = 75.0f;
    MaxRollAngle = 75.0f;
    bEnableRotationLimits = false;  // Disabled by default for full 6DOF

    // Camera properties
    CameraLagSpeed = 3.0f;
    CameraLagMaxDistance = 50.0f;
    BaseFOV = 90.0f;
    BoostFOV = 100.0f;
    FOVInterpSpeed = 5.0f;
    CurrentFOV = BaseFOV;

    // Boost dodge
    BoostDodgeStrength = 2500.0f;
    BoostDodgeDuration = 0.25f;
    BoostDodgeCooldown = 1.0f;
    BoostDodgeCameraShakeScale = 0.5f;

    // State initialization
    PitchInput = 0.0f;
    YawInput = 0.0f;
    ThrottleInput = 0.0f;
    ManualRollInput = 0.0f;
    bIsBraking = false;
    CurrentSpeed = 0.0f;
}

void ASpaceshipBase::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(InputMapping, 0);
        }

        PC->bShowMouseCursor = false;
        PC->bEnableClickEvents = false;
        PC->bEnableMouseOverEvents = false;
        PC->SetInputMode(FInputModeGameOnly());
    }

    // Set initial camera FOV
    if (Camera)
    {
        Camera->SetFieldOfView(BaseFOV);
        CurrentFOV = BaseFOV;
    }

    // Apply camera lag settings
    if (CameraBoom)
    {
        CameraBoom->bEnableCameraLag = true;
        CameraBoom->CameraLagSpeed = CameraLagSpeed;
        CameraBoom->CameraLagMaxDistance = CameraLagMaxDistance;
    }
}

void ASpaceshipBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateBoostDodge(DeltaTime);
    UpdateRotation(DeltaTime);
    UpdateVelocity(DeltaTime);
    UpdateCamera(DeltaTime);
}

void ASpaceshipBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EIC->BindAction(PitchAction, ETriggerEvent::Triggered, this, &ASpaceshipBase::HandlePitch);
        EIC->BindAction(YawAction, ETriggerEvent::Triggered, this, &ASpaceshipBase::HandleYaw);
        EIC->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ASpaceshipBase::HandleThrottle);
        EIC->BindAction(RollAction, ETriggerEvent::Triggered, this, &ASpaceshipBase::HandleRoll);
        EIC->BindAction(BoostDodgeAction, ETriggerEvent::Started, this, &ASpaceshipBase::HandleBoostDodge);
        EIC->BindAction(BrakeAction, ETriggerEvent::Started, this, &ASpaceshipBase::HandleBrake);
        EIC->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ASpaceshipBase::HandleBrakeReleased);
        EIC->BindAction(PrimaryWeaponAction, ETriggerEvent::Triggered, this, &ASpaceshipBase::HandlePrimaryWeapon);
        EIC->BindAction(SecondaryWeaponAction, ETriggerEvent::Started, this, &ASpaceshipBase::HandleSecondaryWeapon);
        EIC->BindAction(CycleWeaponAction, ETriggerEvent::Started, this, &ASpaceshipBase::HandleCycleWeapon);
    }
}

void ASpaceshipBase::HandlePitch(const FInputActionValue& Value)
{
    PitchInput = Value.Get<float>();
}

void ASpaceshipBase::HandleYaw(const FInputActionValue& Value)
{
    YawInput = Value.Get<float>();
}

void ASpaceshipBase::HandleThrottle(const FInputActionValue& Value)
{
    ThrottleInput = Value.Get<float>();
}

void ASpaceshipBase::HandleRoll(const FInputActionValue& Value)
{
    ManualRollInput = Value.Get<float>();
}

void ASpaceshipBase::HandleBoostDodge(const FInputActionValue& Value)
{
    if (!CanBoostDodge()) return;

    FVector2D DodgeInput = Value.Get<FVector2D>();
    if (!DodgeInput.IsNearlyZero())
    {
        // Convert 2D input to 3D world space dodge direction
        FVector Forward = GetActorForwardVector();
        FVector Right = GetActorRightVector();

        BoostDodgeDirection = (Forward * DodgeInput.Y + Right * DodgeInput.X).GetSafeNormal();
        bIsBoostDodging = true;
        BoostDodgeTimer = 0.0f;
        BoostDodgeCooldownTimer = BoostDodgeCooldown;

        // TODO: Add camera shake for boost impact
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            // PC->ClientStartCameraShake(YourCameraShakeClass, BoostDodgeCameraShakeScale);
        }
    }
}

void ASpaceshipBase::HandleBrake(const FInputActionValue& Value)
{
    bIsBraking = true;
}

void ASpaceshipBase::HandleBrakeReleased(const FInputActionValue& Value)
{
    bIsBraking = false;
}

void ASpaceshipBase::HandlePrimaryWeapon(const FInputActionValue& Value)
{
    //if (WeaponSystem)
    //{
    //    WeaponSystem->FirePrimaryWeapon();
    //}
}

void ASpaceshipBase::HandleSecondaryWeapon(const FInputActionValue& Value)
{
    //if (WeaponSystem)
    //{
    //    WeaponSystem->FireSecondaryWeapon();
    //}
}

void ASpaceshipBase::HandleCycleWeapon(const FInputActionValue& Value)
{
    //if (WeaponSystem)
    //{
    //    WeaponSystem->CycleWeapon();
    //}
}

void ASpaceshipBase::UpdateRotation(float DeltaTime)
{
    FRotator CurrentRotation = GetActorRotation();
    FRotator TargetRotationDelta = FRotator::ZeroRotator;

    bool bHasInput = false;

    // Calculate rotation deltas based on input
    if (FMath::Abs(PitchInput) > 0.01f)
    {
        TargetRotationDelta.Pitch = PitchInput * MaxTurnRate * DeltaTime;
        bHasInput = true;
    }

    if (FMath::Abs(YawInput) > 0.01f)
    {
        TargetRotationDelta.Yaw = YawInput * MaxTurnRate * DeltaTime;
        bHasInput = true;
    }

    // Handle manual roll input
    if (FMath::Abs(ManualRollInput) > 0.01f)
    {
        TargetRotationDelta.Roll = ManualRollInput * RollRate * DeltaTime;
        bHasInput = true;

        // When manually rolling, disable auto-level temporarily
        bManualRollActive = true;
    }
    else
    {
        bManualRollActive = false;

        // Optional auto-roll into yaw turns (only when not manually rolling)
        if (bEnableAutoLevel && FMath::Abs(YawInput) > 0.2f)
        {
            TargetRotationDelta.Roll += YawInput * AutoRollStrength * DeltaTime;
            bHasInput = true;
        }
    }

    // Apply rotation using relative rotation (local space)
    if (bHasInput)
    {
        // Use AddActorLocalRotation for proper local space rotation
        AddActorLocalRotation(TargetRotationDelta);

        // Get the new rotation and clamp if needed
        if (bEnableRotationLimits)
        {
            FRotator NewRotation = GetActorRotation();
            NewRotation = ClampRotation(NewRotation);
            SetActorRotation(NewRotation);
        }
    }
    else if (bEnableAutoLevel && !bManualRollActive)
    {
        // Auto-level roll when no input (only if enabled and not manually rolling)
        FRotator CurrentRot = GetActorRotation();

        // Normalize roll to -180 to 180 range for easier interpolation
        float NormalizedRoll = CurrentRot.Roll;
        if (NormalizedRoll > 180.0f) NormalizedRoll -= 360.0f;
        if (NormalizedRoll < -180.0f) NormalizedRoll += 360.0f;

        if (FMath::Abs(NormalizedRoll) > 1.0f)
        {
            FRotator LevelRotation = CurrentRot;
            LevelRotation.Roll = FMath::FInterpTo(NormalizedRoll, 0.0f, DeltaTime, AutoLevelSpeed);

            // Ensure we maintain the normalized range
            if (LevelRotation.Roll > 180.0f) LevelRotation.Roll -= 360.0f;
            if (LevelRotation.Roll < -180.0f) LevelRotation.Roll += 360.0f;

            SetActorRotation(LevelRotation);
        }
    }
}

void ASpaceshipBase::UpdateVelocity(float DeltaTime)
{
    float TargetSpeed;

    if (bIsBraking)
    {
        // Brake to zero
        TargetSpeed = 0.0f;
        CurrentSpeed = FMath::Max(CurrentSpeed - BrakeDeceleration * DeltaTime, 0.0f);
    }
    else
    {
        // Normal throttle control
        TargetSpeed = FMath::Lerp(MinSpeed, MaxSpeed, ThrottleInput);

        if (CurrentSpeed < TargetSpeed)
        {
            CurrentSpeed = FMath::Min(CurrentSpeed + Acceleration * DeltaTime, TargetSpeed);
        }
        else
        {
            CurrentSpeed = FMath::Max(CurrentSpeed - Deceleration * DeltaTime, TargetSpeed);
        }
    }

    FVector BaseVelocity = GetActorForwardVector() * CurrentSpeed;

    // Add boost dodge velocity
    if (bIsBoostDodging)
    {
        float DodgeStrength = FMath::Lerp(BoostDodgeStrength, 0.0f, BoostDodgeTimer / BoostDodgeDuration);
        BaseVelocity += BoostDodgeDirection * DodgeStrength;
    }

    // Move with collision detection
    FVector NewLocation = GetActorLocation() + BaseVelocity * DeltaTime;
    FHitResult HitResult;

    bool bHit = SetActorLocation(NewLocation, true, &HitResult);

    if (bHit && HitResult.bBlockingHit)
    {
        HandleCollision(HitResult);
    }
}

void ASpaceshipBase::UpdateBoostDodge(float DeltaTime)
{
    if (bIsBoostDodging)
    {
        BoostDodgeTimer += DeltaTime;
        if (BoostDodgeTimer >= BoostDodgeDuration)
        {
            bIsBoostDodging = false;
        }
    }

    if (BoostDodgeCooldownTimer > 0.0f)
    {
        BoostDodgeCooldownTimer -= DeltaTime;
    }
}

void ASpaceshipBase::UpdateCamera(float DeltaTime)
{
    if (!Camera) return;

    // Dynamic FOV based on speed and boost
    float TargetFOV = BaseFOV;

    if (bIsBoostDodging)
    {
        TargetFOV = BoostFOV;
    }
    else
    {
        // Subtle FOV increase based on speed
        float SpeedPercent = (CurrentSpeed - MinSpeed) / (MaxSpeed - MinSpeed);
        TargetFOV = FMath::Lerp(BaseFOV, BaseFOV + 5.0f, SpeedPercent);
    }

    CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, FOVInterpSpeed);
    Camera->SetFieldOfView(CurrentFOV);
}

FRotator ASpaceshipBase::ClampRotation(const FRotator& Rotation) const
{
    FRotator ClampedRotation = Rotation;

    // Clamp pitch (up/down)
    if (ClampedRotation.Pitch > MaxPitchAngle && ClampedRotation.Pitch < 180.0f)
    {
        ClampedRotation.Pitch = MaxPitchAngle;
    }
    else if (ClampedRotation.Pitch < 360.0f - MaxPitchAngle && ClampedRotation.Pitch > 180.0f)
    {
        ClampedRotation.Pitch = 360.0f - MaxPitchAngle;
    }

    // Clamp roll (banking)
    if (ClampedRotation.Roll > MaxRollAngle && ClampedRotation.Roll < 180.0f)
    {
        ClampedRotation.Roll = MaxRollAngle;
    }
    else if (ClampedRotation.Roll < 360.0f - MaxRollAngle && ClampedRotation.Roll > 180.0f)
    {
        ClampedRotation.Roll = 360.0f - MaxRollAngle;
    }

    return ClampedRotation;
}

void ASpaceshipBase::HandleCollision(const FHitResult& Hit)
{
    // Basic collision response - bounce back slightly
    if (Hit.bBlockingHit)
    {
        // Reduce speed on impact
        CurrentSpeed *= 0.5f;

        // Optional: Add damage here when health system is implemented
        // TakeDamage(CollisionDamage);

        // Optional: Play impact sound/effect
        // UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Hit.Location);

        // Optional: Camera shake on impact
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            // PC->ClientStartCameraShake(CollisionCameraShakeClass);
        }
    }
}
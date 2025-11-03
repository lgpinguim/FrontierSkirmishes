#include "SpaceshipBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "EnhancedInputComponent.h"  
#include "EnhancedInputSubsystems.h" 
#include "InputActionValue.h"
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

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(ShipRoot);
    CameraBoom->TargetArmLength = 800.0f;
    CameraBoom->bUsePawnControlRotation = false;
    CameraBoom->bInheritPitch = false;
    CameraBoom->bInheritYaw = false;
    CameraBoom->bInheritRoll = false;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(CameraBoom);

    // Create advanced system components
    //LockOnComponent = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOnComponent"));
    //WeaponSystem = CreateDefaultSubobject<UWeaponSystemComponent>(TEXT("WeaponSystem"));
    //TargetLeadComponent = CreateDefaultSubobject<UTargetLeadComponent>(TEXT("TargetLeadComponent"));

    MaxSpeed = 4000.0f;
    MinSpeed = 500.0f;
    Acceleration = 1500.0f;
    Deceleration = 1200.0f;

    MaxTurnRate = 200.0f;
    TurnResponsiveness = 6.0f;
    RollRate = 115.0f;

    MouseSensitivity = 1.0f;
    AutoRollStrength = 60.0f;
    AutoLevelSpeed = 2.0f;
    MaxPitchAngle = 179.0f; 
    MaxYawAngle = 179.0f;   
    MaxRollAngle = 180.0f;  

    BoostDodgeStrength = 2500.0f;
    BoostDodgeDuration = 0.25f;
    BoostDodgeCooldown = 1.0f;
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
}

void ASpaceshipBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateBoostDodge(DeltaTime);
    UpdateRotation(DeltaTime);
    UpdateVelocity(DeltaTime);
}

void ASpaceshipBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EIC->BindAction(MouseAimAction, ETriggerEvent::Triggered, this, &ASpaceshipBase::HandleMouseAim);
        EIC->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ASpaceshipBase::HandleThrottle);
        EIC->BindAction(RollAction, ETriggerEvent::Triggered, this, &ASpaceshipBase::HandleRoll);
        EIC->BindAction(BoostDodgeAction, ETriggerEvent::Started, this, &ASpaceshipBase::HandleBoostDodge);
        EIC->BindAction(PrimaryWeaponAction, ETriggerEvent::Triggered, this, &ASpaceshipBase::HandlePrimaryWeapon);
        EIC->BindAction(SecondaryWeaponAction, ETriggerEvent::Started, this, &ASpaceshipBase::HandleSecondaryWeapon);
        EIC->BindAction(CycleWeaponAction, ETriggerEvent::Started, this, &ASpaceshipBase::HandleCycleWeapon);
    }
}

void ASpaceshipBase::HandleMouseAim(const FInputActionValue& Value)
{
    MouseInput = Value.Get<FVector2D>();
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
        FVector Up = GetActorUpVector();

        BoostDodgeDirection = (Forward * DodgeInput.Y + Right * DodgeInput.X).GetSafeNormal();
        bIsBoostDodging = true;
        BoostDodgeTimer = 0.0f;
        BoostDodgeCooldownTimer = BoostDodgeCooldown;
    }
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
    FRotator RotationDelta = FRotator::ZeroRotator;

    if (!MouseInput.IsNearlyZero(0.01f))
    {
        FVector2D ScaledMouseInput = MouseInput * MouseSensitivity;
        RotationDelta.Yaw = ScaledMouseInput.X * MaxTurnRate * DeltaTime;
        RotationDelta.Pitch = -ScaledMouseInput.Y * MaxTurnRate * DeltaTime;
    }

    RotationDelta.Roll = ManualRollInput * RollRate * DeltaTime;

    if (FMath::Abs(MouseInput.X) > 0.2f && FMath::Abs(ManualRollInput) < 0.1f)
    {
        RotationDelta.Roll += MouseInput.X * AutoRollStrength * DeltaTime;
    }

    if (!RotationDelta.IsZero())
    {
        FRotator TargetRotation = CurrentRotation + RotationDelta;

        TargetRotation.Normalize();

        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, TurnResponsiveness);
        SetActorRotation(NewRotation);
    }
    else
    {
        if (FMath::Abs(CurrentRotation.Roll) > 1.0f)
        {
            FRotator LevelRotation = CurrentRotation;
            LevelRotation.Roll = FMath::FInterpTo(CurrentRotation.Roll, 0.0f, DeltaTime, AutoLevelSpeed);
            SetActorRotation(LevelRotation);
        }
    }

    MouseInput = FVector2D::ZeroVector;
}

void ASpaceshipBase::UpdateVelocity(float DeltaTime)
{
    float TargetSpeed = FMath::Lerp(MinSpeed, MaxSpeed, ThrottleInput);

    if (CurrentSpeed < TargetSpeed)
    {
        CurrentSpeed = FMath::Min(CurrentSpeed + Acceleration * DeltaTime, TargetSpeed);
    }
    else
    {
        CurrentSpeed = FMath::Max(CurrentSpeed - Deceleration * DeltaTime, TargetSpeed);
    }

    FVector BaseVelocity = GetActorForwardVector() * CurrentSpeed;

    if (bIsBoostDodging)
    {
        float DodgeStrength = FMath::Lerp(BoostDodgeStrength, 0.0f, BoostDodgeTimer / BoostDodgeDuration);
        BaseVelocity += BoostDodgeDirection * DodgeStrength;
    }

    CurrentVelocity = BaseVelocity;

    FVector NewLocation = GetActorLocation() + CurrentVelocity * DeltaTime;
    SetActorLocation(NewLocation, true);
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

FRotator ASpaceshipBase::SmoothRotateTowards(const FRotator& Current, const FRotator& Target, float DeltaTime, float InterpSpeed)
{
    return FMath::RInterpTo(Current, Target, DeltaTime, InterpSpeed);
}
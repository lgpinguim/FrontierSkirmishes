// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceshipBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "EnhancedInputComponent.h"  // NEW
#include "EnhancedInputSubsystems.h"  // NEW
#include "InputActionValue.h"  // NEW

// Sets default values
ASpaceshipBase::ASpaceshipBase()
{
    // Enable Tick (called every frame)
    PrimaryActorTick.bCanEverTick = true;

    // CREATE COMPONENTS

    // Root component - invisible point that everything attaches to
    ShipRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ShipRoot"));
    RootComponent = ShipRoot;

    // Ship mesh - the visual representation
    ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
    ShipMesh->SetupAttachment(ShipRoot);

    // Camera boom - keeps camera at distance behind ship
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(ShipRoot);
    CameraBoom->TargetArmLength = 800.0f; // Distance behind ship
    CameraBoom->bUsePawnControlRotation = false; // Camera follows ship rotation
    CameraBoom->bInheritPitch = true;
    CameraBoom->bInheritYaw = true;
    CameraBoom->bInheritRoll = true;

    // Camera - what the player sees through
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(CameraBoom);

    // SET DEFAULT MOVEMENT VALUES (tweakable in editor)

    ThrustPower = 2000.0f;      // Acceleration force
    MaxSpeed = 3000.0f;         // Top speed
    Drag = 0.95f;               // How quickly ship slows (0.95 = 5% speed loss per frame)
    PitchSpeed = 100.0f;        // Rotation speed up/down
    YawSpeed = 100.0f;          // Rotation speed left/right
    RollSpeed = 150.0f;         // Rotation speed for barrel rolls

    CurrentVelocity = FVector::ZeroVector; // Start stationary

}

// Called when the game starts or when spawned
void ASpaceshipBase::BeginPlay()
{
	Super::BeginPlay();

    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(InputMapping, 0);
        }
    }
	
}

// Called every frame
void ASpaceshipBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CurrentVelocity *= Drag;

    if (CurrentVelocity.Size() > MaxSpeed)
    {
        CurrentVelocity = CurrentVelocity.GetSafeNormal() * MaxSpeed;
    }

    FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
    SetActorLocation(NewLocation);
}

// Called to bind functionality to input
void ASpaceshipBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Cast to Enhanced Input Component
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Bind actions
        EnhancedInputComponent->BindAction(ThrustAction, ETriggerEvent::Triggered, this, &ASpaceshipBase::ThrustInput);
        EnhancedInputComponent->BindAction(PitchAction, ETriggerEvent::Triggered, this, &ASpaceshipBase::PitchInput);
        EnhancedInputComponent->BindAction(YawAction, ETriggerEvent::Triggered, this, &ASpaceshipBase::YawInput);
        EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &ASpaceshipBase::RollInput);
    }
}

void ASpaceshipBase::ThrustInput(const FInputActionValue& Value)
{
    float InputValue = Value.Get<float>();

    if (InputValue != 0.0f)
    {
        FVector ForwardDirection = GetActorForwardVector();
        CurrentVelocity += ForwardDirection * ThrustPower * InputValue * GetWorld()->GetDeltaSeconds();
    }
}

void ASpaceshipBase::PitchInput(const FInputActionValue& Value)
{
    float InputValue = Value.Get<float>();

    if (InputValue != 0.0f)
    {
        float DeltaTime = GetWorld()->GetDeltaSeconds();
        float PitchAmount = InputValue * PitchSpeed * DeltaTime;

        FRotator NewRotation = GetActorRotation();
        NewRotation.Pitch += PitchAmount;
        SetActorRotation(NewRotation);
    }
}

void ASpaceshipBase::YawInput(const FInputActionValue& Value)
{
    float InputValue = Value.Get<float>();

    if (InputValue != 0.0f)
    {
        float DeltaTime = GetWorld()->GetDeltaSeconds();
        float YawAmount = InputValue * YawSpeed * DeltaTime;

        FRotator NewRotation = GetActorRotation();
        NewRotation.Yaw += YawAmount;
        SetActorRotation(NewRotation);
    }
}

void ASpaceshipBase::RollInput(const FInputActionValue& Value)
{
    float InputValue = Value.Get<float>();

    if (InputValue != 0.0f)
    {
        float DeltaTime = GetWorld()->GetDeltaSeconds();
        float RollAmount = InputValue * RollSpeed * DeltaTime;

        FRotator NewRotation = GetActorRotation();
        NewRotation.Roll += RollAmount;
        SetActorRotation(NewRotation);
    }
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "StarcraftBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"

AStarcraftBase::AStarcraftBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	
	// CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	// CameraBoom->SetupAttachment(GetRootComponent());
	// CameraBoom->TargetArmLength = 600.0f; //Edit this value to around 2,5K
	// CameraBoom->bUsePawnControlRotation = true;
	//
	// FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	// FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// FollowCamera->bUsePawnControlRotation = false;
	
}

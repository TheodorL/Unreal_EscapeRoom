// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "PlayerCharMovementComp.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PlayerCharController.h"
#include "Engine/World.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlayerCharMovementComp>(ACharacter::CharacterMovementComponentName))
{
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	BaseYawRate = 540.0f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, BaseYawRate, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.5f;
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->bApplyGravityWhileJumping = true;
	GetCharacterMovement()->Mass = 1.0f;


	CameraComponent = CreateDefaultSubobject<UCameraComponent>(FName("CameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->bUsePawnControlRotation = true;

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(FName("PhysicsHandle"));

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CharController = Cast<APlayerCharController>(GetController());
	TempReach = InitReach;
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (PhysicsHandle->GrabbedComponent && !bIsThrowing)
	{
		UpdateGrabbedComponent();
	}

}

void APlayerCharacter::UpdateGrabbedComponent()
{
	PhysicsHandle->SetTargetLocation(CameraComponent->GetComponentLocation() + CameraComponent->GetComponentRotation().Vector() * TempReach);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Grab", IE_Pressed, this, &APlayerCharacter::TryGrab);
	PlayerInputComponent->BindAction("Grab", IE_Released, this, &APlayerCharacter::Release);
	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &APlayerCharacter::Throw);

	PlayerInputComponent->BindAxis("InputForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("InputRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("IncreaseReach", this, &APlayerCharacter::IncreaseReach);

}

void APlayerCharacter::MoveForward(float Scale)
{
	if (FMath::IsNearlyZero(Scale)) return;
	const FRotator YawRotation(0.0f, GetControlRotation().Yaw , 0.0f);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	GetMovementComponent()->AddInputVector(Scale * Direction);
}

void APlayerCharacter::MoveRight(float Scale)
{
	if (FMath::IsNearlyZero(Scale)) return;
	const FRotator YawRotation(0.0f, GetControlRotation().Yaw, 0.0f);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	GetMovementComponent()->AddInputVector(Scale*Direction);
}

void APlayerCharacter::TurnAtRate(float Scale)
{
	AddControllerYawInput(Scale * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Scale)
{
	AddControllerPitchInput(Scale * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::TryGrab()
{
	FVector StartVector = CameraComponent->GetComponentLocation();
	FVector EndVector = StartVector + CameraComponent->GetComponentRotation().Vector() * InitReach;
	FHitResult LineTraceResult;
	GetWorld()->LineTraceSingleByObjectType
	(
		LineTraceResult,
		StartVector,
		EndVector,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		FCollisionQueryParams(FName("Text"), false, this)
	);
	if (LineTraceResult.GetComponent())
	{
		Grab(LineTraceResult.GetComponent());
	}
}

void APlayerCharacter::Grab(UPrimitiveComponent* ComponentToGrab)
{
	PhysicsHandle->GrabComponentAtLocationWithRotation
	(
		ComponentToGrab,
		NAME_None,
		ComponentToGrab->GetOwner()->GetActorLocation(),
		ComponentToGrab->GetOwner()->GetActorRotation()
	);
}

void APlayerCharacter::Release()
{
	PhysicsHandle->ReleaseComponent();
}

void APlayerCharacter::IncreaseReach(float Scale)
{
	if (PhysicsHandle->GrabbedComponent)
	{
		TempReach += Scale * DeltaReach;
		TempReach = FMath::Clamp(TempReach, 75.0f, 185.0f);
	}
}

void APlayerCharacter::Throw()
{
	if (PhysicsHandle->GrabbedComponent)
	{
		bIsThrowing = true;
		PhysicsHandle->GrabbedComponent->AddImpulse(FVector(CameraComponent->GetComponentLocation() + CameraComponent->GetComponentRotation().Vector() * ThrowForce));
		PhysicsHandle->ReleaseComponent();
		bIsThrowing = false;
	}
}
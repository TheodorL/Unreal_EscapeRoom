// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "PlayerCharMovementComp.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PlayerCharController.h"
#include "Engine/World.h"
#include "GunBase.h"
#include "Engine/StaticMesh.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectGlobals.h"

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

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->MaxWalkSpeed = StepSpeed;

	if (!CameraComponent)
	{
		CameraComponent = CreateDefaultSubobject<UCameraComponent>(FName("CameraComponent"));
		CameraComponent->SetupAttachment(RootComponent);
		CameraComponent->bUsePawnControlRotation = true;
	}

	if (!PhysicsHandle)
	{
		PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(FName("PhysicsHandle"));
	}

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
	
	if (PhysicsHandle->GrabbedComponent)// && !bIsThrowing)
	{
		UpdateGrabbedComponent();
	}

}

void APlayerCharacter::UpdateGrabbedComponent()
{
	PhysicsHandle->SetTargetLocation(this->GetActorLocation() + CameraComponent->GetComponentRotation().Vector() * TempReach);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::StopSprinting);

	PlayerInputComponent->BindAxis("InputForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("InputRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("IncreaseReach", this, &APlayerCharacter::IncreaseReach);


	PlayerInputComponent->BindAction("Grab", IE_Pressed, this, &APlayerCharacter::OnRightClickPressed);
	PlayerInputComponent->BindAction("Grab", IE_Released, this, &APlayerCharacter::OnRightClickReleased);
	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &APlayerCharacter::OnLeftClickPressed);

	PlayerInputComponent->BindAction("CreateGun", IE_Pressed, this, &APlayerCharacter::CreateGun);
	PlayerInputComponent->BindAction("DeleteGun", IE_Pressed, this, &APlayerCharacter::DeleteGun);
}



void APlayerCharacter::MoveForward(float Scale)
{
	AddMovementInput(GetActorForwardVector(), Scale);
}

void APlayerCharacter::MoveRight(float Scale)
{
	AddMovementInput(GetActorRightVector(), Scale);
}

void APlayerCharacter::TurnAtRate(float Scale)
{
	AddControllerYawInput(Scale * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Scale)
{
	AddControllerPitchInput(Scale * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::StartSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void APlayerCharacter::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = StepSpeed;
}

void APlayerCharacter::OnRightClickPressed()
{
	if (CharacterGun) return;
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
	if (LineTraceResult.GetActor())
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
	if(PhysicsHandle->GrabbedComponent)
		PhysicsHandle->GrabbedComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

void APlayerCharacter::OnRightClickReleased()
{
	if (!PhysicsHandle->GrabbedComponent) return;
	PhysicsHandle->GrabbedComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
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

void APlayerCharacter::OnLeftClickPressed()
{
	if (CharacterGun)
	{
		CharacterGun->StartFiring();
	}
	else if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->GrabbedComponent->AddImpulse(FVector(CameraComponent->GetComponentLocation() + CameraComponent->GetComponentRotation().Vector() * ThrowForce));
		OnRightClickReleased();
	}
}

void APlayerCharacter::CreateGun()
{
	if(!CharacterGun)
	{
		CharacterGun = NewObject<UGunBase>(this, UGunBase::StaticClass(), FName("CharacterGun"), EObjectFlags::RF_Dynamic);
		CharacterGun->InitializeWeapon(EWeaponType::WT_2);
		CharacterGun->RegisterComponent();
		CharacterGun->AttachToComponent(CameraComponent, FAttachmentTransformRules::KeepRelativeTransform);
		CharacterGun->SetRelativeLocation(FVector(20.0f, 20.0f, 10.0f));
		UE_LOG(LogTemp, Warning, TEXT("Creating gun!"))
		}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterGun already exists!"))
	}
}

void APlayerCharacter::DeleteGun()
{
	if (!CharacterGun) return;
	CharacterGun->UnregisterComponent();
	CharacterGun->DestroyComponent();
	CharacterGun = nullptr;
	UE_LOG(LogTemp, Warning, TEXT("Deleting gun!"))
}
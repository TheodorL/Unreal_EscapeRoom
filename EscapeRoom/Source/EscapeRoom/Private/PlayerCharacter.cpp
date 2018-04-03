// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "PlayerCharMovementComp.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PlayerCharController.h"
#include "Engine/World.h"
#include "GunBase.h"
#include "InteractiveMesh.h"

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

	CurrentWeaponType = EWeaponType::WT_None;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CharController = Cast<APlayerCharController>(GetController());
	TempReach = InitReach;

	PhysicsHandle->SetLinearDamping(0.0f);
	PhysicsHandle->SetLinearStiffness(15000.0f);
	PhysicsHandle->SetAngularDamping(0.0f);
	PhysicsHandle->SetAngularStiffness(15000.0f);
	PhysicsHandle->SetInterpolationSpeed(15000.0f);

}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (PhysicsHandle->GrabbedComponent)
	{
		UpdateGrabbedComponentLocation();
	}

}

void APlayerCharacter::UpdateGrabbedComponentLocation()
{
	PhysicsHandle->SetTargetLocation(this->GetActorLocation() + CameraComponent->GetComponentRotation().Vector() * TempReach);	

	FVector CurrentLocation = PhysicsHandle->GrabbedComponent->GetComponentLocation();
	if (FMath::Abs(CurrentLocation.X - CameraComponent->GetComponentLocation().X) > MaxReach || FMath::Abs(CurrentLocation.Y - CameraComponent->GetComponentLocation().Y) >= MaxReach || FMath::Abs(CurrentLocation.Z - CameraComponent->GetComponentLocation().Z) >= MaxReach)
	{
		ReleaseGrabbedComponent();
	}

}

void APlayerCharacter::GrabbedComponentAddRotation(FRotator RotationToAdd)
{
	FRotator RotationToSet = PhysicsHandle->GrabbedComponent->GetComponentRotation() + RotationToAdd;
	PhysicsHandle->SetTargetRotation(RotationToSet);
	RotationToAdd = FRotator(0.0f);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::StopSprinting);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::InteractButton);
	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &APlayerCharacter::FireButton);
	PlayerInputComponent->BindAction("Throw", IE_Released, this, &APlayerCharacter::OnLeftClickReleased);
	PlayerInputComponent->BindAction("ToggleRotation", IE_Pressed, this, &APlayerCharacter::ToggleRotation);

	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &APlayerCharacter::DropKeyPressed);
	PlayerInputComponent->BindAction("Key1", IE_Pressed, this, &APlayerCharacter::Key1Pressed);
	PlayerInputComponent->BindAction("Key2", IE_Pressed, this, &APlayerCharacter::Key2Pressed);

	PlayerInputComponent->BindAxis("InputForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("InputRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("IncreaseReach", this, &APlayerCharacter::IncreaseReach);

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

	if(PhysicsHandle->GrabbedComponent && bIsRotating)
	{
		GrabbedComponentAddRotation(FRotator(0.0f, Scale*BaseTurnRate*GetWorld()->GetDeltaSeconds(), 0.0f));
	}
	else
	{
		AddControllerYawInput(Scale * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
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

void APlayerCharacter::InteractButton()
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
	AActor * ResultActor = LineTraceResult.GetActor();
	if (ResultActor)
	{
		AInteractiveMesh* Temp = Cast<AInteractiveMesh>(ResultActor);
		if (Temp)
		{
			switch (Temp->GetMeshType())
			{
			case EMeshType::MT_None:
				Grab(LineTraceResult.GetComponent());
				break;
			case EMeshType::MT_WT_Semi:
				CurrentWeaponType = EWeaponType::WT_Semi;
				CreateGun();
				break;
			case EMeshType::MT_WT_Auto:
				CurrentWeaponType = EWeaponType::WT_Auto;
				CreateGun();
				break;
			}

			Temp->Destroy();
		}
		else
		{
			Grab(LineTraceResult.GetComponent());
		}
		
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
	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->GrabbedComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	}
}

void APlayerCharacter::ToggleRotation()
{
	if (bIsRotating)
		bIsRotating = false;
	else
		bIsRotating = true;
}

void APlayerCharacter::IncreaseReach(float Scale)
{
	if (PhysicsHandle->GrabbedComponent)
	{
		TempReach += Scale * DeltaReach;
		TempReach = FMath::Clamp(TempReach, 75.0f, MaxReach);
	}
}

void APlayerCharacter::FireButton()
{
	if (CharacterGun)
	{
		CharacterGun->StartFiring();
	}
	else if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->GrabbedComponent->AddImpulse(FVector(CameraComponent->GetComponentLocation() + CameraComponent->GetComponentRotation().Vector() * ThrowForce));
		ReleaseGrabbedComponent();
	}
}

void APlayerCharacter::OnLeftClickReleased()
{
	if (CharacterGun)
	{
		CharacterGun->StopFiring();
	}
}

void APlayerCharacter::CreateGun()
{
	if(!CharacterGun && CurrentWeaponType != EWeaponType::WT_None && !PhysicsHandle->GrabbedComponent)
	{
		CharacterGun = NewObject<UGunBase>(this, UGunBase::StaticClass(), FName("CharacterGun"), EObjectFlags::RF_Dynamic);
		CharacterGun->InitializeWeapon(CurrentWeaponType);
		CharacterGun->RegisterComponent();
		CharacterGun->AttachToComponent(CameraComponent, FAttachmentTransformRules::KeepRelativeTransform);
		CharacterGun->SetRelativeLocation(FVector(20.0f, 20.0f, 0.0f));
		}
}

void APlayerCharacter::DeleteGun()
{
	if (!CharacterGun) return;
	CharacterGun->StopFiring();
	CharacterGun->UnregisterComponent();
	CharacterGun = nullptr;
}

void APlayerCharacter::Key1Pressed()
{
	if (CurrentWeaponType == EWeaponType::WT_Semi || !CharacterGun) return;

	CurrentWeaponType = EWeaponType::WT_Semi;
	CharacterGun->InitializeWeapon(CurrentWeaponType);
}

void APlayerCharacter::Key2Pressed()
{
	if (CurrentWeaponType == EWeaponType::WT_Auto || !CharacterGun) return;

	CurrentWeaponType = EWeaponType::WT_Auto;
	CharacterGun->InitializeWeapon(CurrentWeaponType);
}

void APlayerCharacter::ReleaseGrabbedComponent()
{
	PhysicsHandle->GrabbedComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	PhysicsHandle->ReleaseComponent();
	bIsRotating = false;
}

void APlayerCharacter::DropKeyPressed()
{
	if (CharacterGun)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AInteractiveMesh * Temp = GetWorld()->SpawnActor<AInteractiveMesh>(AInteractiveMesh::StaticClass(), this->GetActorLocation() + CameraComponent->GetComponentRotation().Vector() * TempReach, FRotator(0.0f), SpawnParameters);
		switch (CurrentWeaponType)
		{
		case EWeaponType::WT_Semi :
			Temp->InitializeMesh(EMeshType::MT_WT_Semi);
			break;
		case EWeaponType::WT_Auto :
			Temp->InitializeMesh(EMeshType::MT_WT_Auto);
			break;
		}
		DeleteGun();	
	}
	else if(PhysicsHandle->GrabbedComponent)
	{
		ReleaseGrabbedComponent();
	}
}

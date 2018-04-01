// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class UPhysicsHandleComponent;
class APlayerCharController;
class UGunBase;
class UStaticMesh;
enum class EWeaponType : uint8;

UCLASS()
class ESCAPEROOM_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly)
		float ThrowForce = 50000.0f; //in CentiNewtons probably


	UPROPERTY(EditAnywhere)
		bool bShouldHaveGun = true;

	UPROPERTY()
		UGunBase * CharacterGun = nullptr;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void UpdateGrabbedComponent();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UCameraComponent * CameraComponent = nullptr;
	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	APlayerCharController * CharController = nullptr;

	void MoveForward(float Scale);
	void MoveRight(float Scale);
	void TurnAtRate(float Scale);
	void LookUpAtRate(float Scale);
	void StartSprinting();
	void StopSprinting();

	void OnRightClickPressed();
	void Grab(UPrimitiveComponent* ComponentToGrab);
	void OnRightClickReleased();
	void IncreaseReach(float Scale);
	void OnLeftClickPressed();
	void OnLeftClickReleased();

	void CreateGun();
	void DeleteGun();
	void Key1Pressed();
	void Key2Pressed();

	UPROPERTY(EditDefaultsOnly)
		float BaseTurnRate = 45.0f;

	UPROPERTY(EditDefaultsOnly)
		float BaseLookUpRate = 45.0f;

	UPROPERTY(EditDefaultsOnly)
		float InitReach = 120.0f;

	UPROPERTY(EditDefaultsOnly)
		float DeltaReach = 2.0f;

	float TempReach;

	UPROPERTY(EditDefaultsOnly)
		float StepSpeed = 350.0f;

	UPROPERTY(EditDefaultsOnly)
		float SprintSpeed = 600.0f;

	bool bIsThrowing = false;
	bool bIsSprinting = false;

	EWeaponType CurrentWeaponType;
	
};

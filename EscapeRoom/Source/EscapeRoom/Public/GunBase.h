// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "GunBase.generated.h"

struct FTimerHandle;

UENUM()
enum class EWeaponType : uint8
{
	WT_1,
	WT_2
};

/**
 * 
 */
UCLASS(Blueprintable)
class ESCAPEROOM_API UGunBase : public UStaticMeshComponent
{
	GENERATED_BODY()


public:

	UGunBase();

	virtual void BeginPlay() override;

	void InitializeWeapon(EWeaponType WeaponType);

	void StartFiring();
	void StopFiring();
	void Fire();
	
private:
	bool bIsAutomatic;

	float LastFireTime;
	float TimeBetweenFires;

	FTimerHandle TimerHandle;


	

	
};

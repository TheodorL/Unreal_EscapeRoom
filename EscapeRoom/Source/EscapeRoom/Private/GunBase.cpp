// Fill out your copyright notice in the Description page of Project Settings.

#include "GunBase.h"
#include "Engine/World.h"
#include "BaseProjectile.h"
#include "Classes/Engine/StaticMesh.h"
#include "TimerManager.h"

UGunBase::UGunBase()
{
	bIsAutomatic = false;
	TimeBetweenFires = 1.0f;
	LastFireTime = TimeBetweenFires;
	PrimaryComponentTick.bCanEverTick = true;
	TimerHandle = FTimerHandle();
}

void UGunBase::BeginPlay()
{
	
}

void UGunBase::InitializeWeapon(EWeaponType WeaponType)
{
	switch (WeaponType)
	{
		case EWeaponType::WT_1 : 
			SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Shape_Cube")));
			bIsAutomatic = false;
			break;
		case EWeaponType::WT_2 : 
			SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Shape_Cone")));
			bIsAutomatic = true;
			break;
		default: break;
	}
	SetRelativeScale3D(FVector(0.125f, 0.125f, 0.125f));
}


void UGunBase::StartFiring()
{
	if (bIsAutomatic)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UGunBase::Fire, 0.05f, true, 0.25f);
	}
	Fire();
}

void UGunBase::StopFiring()
{
	if (bIsAutomatic)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
}

void UGunBase::Fire()
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	auto CurrentProjectile = GetWorld()->SpawnActor<ABaseProjectile>(ABaseProjectile::StaticClass(), GetSocketLocation(FName("ProjectileSocket")), GetSocketRotation(FName("ProjectileSocket")));
	CurrentProjectile->Launch();
}



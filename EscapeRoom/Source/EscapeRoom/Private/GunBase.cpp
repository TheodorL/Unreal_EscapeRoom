// Fill out your copyright notice in the Description page of Project Settings.

#include "GunBase.h"
#include "Engine/World.h"
#include "BaseProjectile.h"
#include "Classes/Engine/StaticMesh.h"

UGunBase::UGunBase()
{
	SetVisibility(true);
}

void UGunBase::InitializeWeapon(EWeaponType WeaponType)
{
	if (WeaponType == EWeaponType::WT_1)
	{
		SetStaticMesh(LoadObject<UStaticMesh>(this, TEXT("/Game/Shape_Cube")));
	}
	else
	{
		SetStaticMesh(LoadObject<UStaticMesh>(this, TEXT("/Game/Shape_Cone")));
	}
}


void UGunBase::StartFiring()
{
	UE_LOG(LogTemp, Warning, TEXT("Firing!"))

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	auto CurrentProjectile = GetWorld()->SpawnActor<ABaseProjectile>(ABaseProjectile::StaticClass(), GetSocketLocation(FName("ProjectileSocket")), GetSocketRotation(FName("ProjectileSocket")));
	CurrentProjectile->Launch();
}

void UGunBase::StopFiring()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop Firing!"))
}



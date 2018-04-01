// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharMovementComp.h"

UPlayerCharMovementComp::UPlayerCharMovementComp()
{
	RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	JumpZVelocity = 600.0f;
	AirControl = 0.5f;
	GravityScale = 2.0f;
	bApplyGravityWhileJumping = true;
	Mass = 1.0f;
	bOrientRotationToMovement = false;
}



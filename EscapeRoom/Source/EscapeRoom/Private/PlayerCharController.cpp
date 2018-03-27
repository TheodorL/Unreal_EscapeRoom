// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharController.h"
#include "PlayerCharacter.h"


void APlayerCharController::BeginPlay()
{
	Super::BeginPlay();
	PossessedCharacter = Cast<APlayerCharacter>(GetCharacter());
}

void APlayerCharController::SetupInputComponent()
{
}


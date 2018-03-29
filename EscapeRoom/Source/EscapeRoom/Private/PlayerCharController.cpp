// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharController.h"
#include "PlayerCharacter.h"
#include "Blueprint/UserWidget.h"


void APlayerCharController::BeginPlay()
{
	Super::BeginPlay();
	PossessedCharacter = Cast<APlayerCharacter>(GetCharacter());

	if (wHUD)
	{
		HUD = CreateWidget<UUserWidget>(this, wHUD);
		if (HUD)
		{
			HUD->AddToViewport();
		}
	}
}

void APlayerCharController::SetupInputComponent()
{
}


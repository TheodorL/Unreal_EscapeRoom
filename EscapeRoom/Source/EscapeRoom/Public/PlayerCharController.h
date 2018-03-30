// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharController.generated.h"

class APlayerCharacter;
class UUserWidget;
class UInputComponent;

/**
 * 
 */
UCLASS()
class ESCAPEROOM_API APlayerCharController : public APlayerController
{
	GENERATED_BODY()

public:
	

protected:
	virtual void BeginPlay() override;


};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharController.generated.h"

class APlayerCharacter;
class UUserWidget;

/**
 * 
 */
UCLASS()
class ESCAPEROOM_API APlayerCharController : public APlayerController
{
	GENERATED_BODY()

public:

	//reference to the editor widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<UUserWidget> wHUD;

	UUserWidget * HUD = nullptr;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:

	APlayerCharacter* PossessedCharacter = nullptr;

	
	
};

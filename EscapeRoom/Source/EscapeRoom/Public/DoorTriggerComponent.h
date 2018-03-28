// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DoorTriggerComponent.generated.h"

class ATriggerVolume;
class UStaticMeshComponent;
class UCurveFloat;
struct FTimeline;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDoorDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ESCAPEROOM_API UDoorTriggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDoorTriggerComponent();


	// Events handling opening and closing doors
	// Used in order to avoid latency inside the TickComponent method
	UPROPERTY(BlueprintAssignable)
		FDoorDelegate OpenDoorEvent;
	UPROPERTY(BlueprintAssignable)
		FDoorDelegate CloseDoorEvent;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UCurveFloat * LoadCurveFloatAsset(const TCHAR * ASSET_PATH);
	void MaintainTimeline(FTimeline * Timeline, UCurveFloat * CurveFloat, const FName & MethodName, bool Looping);

	UPROPERTY(EditAnywhere)
		ATriggerVolume* PressurePlate = nullptr;

	UPROPERTY(EditAnywhere)
		float MassTreshold = 30.0f;

	float InitYaw;
	float FinalYaw;
	bool bIsDoorOpen = false;

	float GetMassOfActorsInVolume();

	UFUNCTION()
	void OpenDoor();

	UFUNCTION()
	void CloseDoor();
	
};

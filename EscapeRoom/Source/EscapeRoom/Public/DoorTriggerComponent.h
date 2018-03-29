// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DoorTriggerComponent.generated.h"

//Forward declarations
class ATriggerVolume;
class UStaticMeshComponent;
class UCurveFloat;
struct FTimeline;
class UBoxComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ESCAPEROOM_API UDoorTriggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDoorTriggerComponent();

	UPROPERTY(EditAnywhere)
	FVector TriggerScale = FVector(80.0f, 80.0f, 80.0f);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	//returns the curvefloat found at the given path
	static UCurveFloat * LoadCurveFloatAsset(const TCHAR * ASSET_PATH);

	//binds a curve float and a interp function to the timeline
	void MaintainTimeline(FTimeline * Timeline, UCurveFloat * CurveFloat, const FName & MethodName, bool Looping);

	UPROPERTY(EditAnywhere)
		float MassTreshold = 10.0f;

	float InitYaw;
	bool bIsDoorOpen = false;

	//uses a timeline function in order to rotate the door
	//adds rotation!
	UFUNCTION()
		void RotateDoorUsingTimeline(float Value);

	FTimeline* FTLDoorOpening = nullptr;

	float TotalMass;

	UPROPERTY(EditAnywhere)
	UBoxComponent * BoxTrigger = nullptr;

	UFUNCTION()
	void OnBoxBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnBoxEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};

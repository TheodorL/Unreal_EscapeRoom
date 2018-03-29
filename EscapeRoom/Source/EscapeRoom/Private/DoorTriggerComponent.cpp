// Fill out your copyright notice in the Description page of Project Settings.

#include "DoorTriggerComponent.h"
#include "Engine/TriggerVolume.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "ConstructorHelpers.h"

static UCurveFloat* OpenDoorCurve = nullptr;


// Sets default values for this component's properties
UDoorTriggerComponent::UDoorTriggerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	if (OpenDoorCurve == nullptr)
	{
		OpenDoorCurve = LoadCurveFloatAsset(TEXT("/Game/DoorOpening"));
	}

	FTLDoorOpening = new FTimeline;
}

void UDoorTriggerComponent::BeginDestroy()
{
	delete FTLDoorOpening;

	Super::BeginDestroy();
}


// Called when the game starts
void UDoorTriggerComponent::BeginPlay()
{
	Super::BeginPlay();
	InitYaw = GetOwner()->GetActorRotation().Yaw;

	if (OpenDoorCurve)
	{
		UE_LOG(LogTemp, Warning, TEXT("YES"))
		MaintainTimeline(FTLDoorOpening, OpenDoorCurve, FName("RotateDoorUsingTimeline"), false);
	}

	
	
}


// Called every frame
void UDoorTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FTLDoorOpening->TickTimeline(DeltaTime);
	

	if (GetMassOfActorsInVolume() > MassTreshold && bIsDoorOpen == false)
	{
		FTLDoorOpening->Play();
		bIsDoorOpen = true;
	}
	if (GetMassOfActorsInVolume() < MassTreshold && bIsDoorOpen == true)
	{
		FTLDoorOpening->Reverse();
		bIsDoorOpen = false;
	}
}

float UDoorTriggerComponent::GetMassOfActorsInVolume()
{
	if (!PressurePlate) return -1.0f;
	float TotalMass = 0.0f;
	TArray<AActor*> ActorsInVolume;
	PressurePlate->GetOverlappingActors(ActorsInVolume);
	for (const AActor *Actor : ActorsInVolume)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	return TotalMass;
}

UCurveFloat * UDoorTriggerComponent::LoadCurveFloatAsset(const TCHAR * ASSET_PATH)
{
	ConstructorHelpers::FObjectFinder<UCurveFloat> CurveAsset(ASSET_PATH);

	if (CurveAsset.Succeeded())
	{
		return CurveAsset.Object;
	}
	return nullptr;
}

void UDoorTriggerComponent::MaintainTimeline(FTimeline * Timeline, UCurveFloat * CurveFloat, const FName & MethodName, bool Looping = false)
{
	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, MethodName);

	Timeline->AddInterpFloat(CurveFloat, ProgressFunction);
	Timeline->SetLooping(Looping);
}

void UDoorTriggerComponent::RotateDoorUsingTimeline(float Value)
{
	float YawToSet = Value * 90.0f;
	GetOwner()->SetActorRotation(FRotator(0.0f, InitYaw + YawToSet, 0.0f));
}


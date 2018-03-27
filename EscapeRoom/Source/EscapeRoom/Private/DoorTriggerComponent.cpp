// Fill out your copyright notice in the Description page of Project Settings.

#include "DoorTriggerComponent.h"
#include "Engine/TriggerVolume.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"


// Sets default values for this component's properties
UDoorTriggerComponent::UDoorTriggerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UDoorTriggerComponent::BeginPlay()
{
	Super::BeginPlay();
	InitYaw = GetOwner()->GetActorRotation().Yaw;
	FinalYaw = InitYaw + 90.0f;
	
}


// Called every frame
void UDoorTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GetMassOfActorsInVolume() > MassTreshold)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpeningDoor"));
		OpenDoor();
	}
	if (GetMassOfActorsInVolume() < MassTreshold)
	{
		CloseDoor();
	}
}

float UDoorTriggerComponent::GetMassOfActorsInVolume()
{
	if (!PressurePlate) return -1.0f;
	float TotalMass = 0.0f;
	TArray<AActor*> ActorsInVolume;
	PressurePlate->GetOverlappingActors(ActorsInVolume);
	for (const auto &Actor : ActorsInVolume)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	return TotalMass;
}

void UDoorTriggerComponent::OpenDoor()
{
		GetOwner()->AddActorWorldRotation(FRotator(0.0f, 1.0f, 0.0f));
	bIsDoorOpen = true;
}

void UDoorTriggerComponent::CloseDoor()
{
		GetOwner()->AddActorWorldRotation(FRotator(0.0f, -1.0f, 0.0f));
	bIsDoorOpen = false;
}


// Fill out your copyright notice in the Description page of Project Settings.

#include "DoorTriggerComponent.h"
#include "Engine/TriggerVolume.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "ConstructorHelpers.h"
#include "Components/BoxComponent.h"

static UCurveFloat* OpenDoorCurve = nullptr;


// Sets default values for this component's properties
UDoorTriggerComponent::UDoorTriggerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TotalMass = 0;

	if (OpenDoorCurve == nullptr)
	{
		OpenDoorCurve = LoadCurveFloatAsset(TEXT("/Game/DoorOpening"));
	}

	FTLDoorOpening = new FTimeline;
	if (!BoxTrigger)
	{
		BoxTrigger = CreateDefaultSubobject<UBoxComponent>(FName("DoorTriggerArea"));

		if (GetOwner())
		{
			BoxTrigger->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		}

		BoxTrigger->InitBoxExtent(TriggerScale);
		BoxTrigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		BoxTrigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		BoxTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
		BoxTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		BoxTrigger->bGenerateOverlapEvents = true;
		BoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &UDoorTriggerComponent::OnBoxBeginOverlap);
		BoxTrigger->OnComponentEndOverlap.AddDynamic(this, &UDoorTriggerComponent::OnBoxEndOverlap);
	}

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

	if (TotalMass > MassTreshold && bIsDoorOpen == false)
	{
		FTLDoorOpening->Play();
		bIsDoorOpen = true;
	}
	if (TotalMass < MassTreshold && bIsDoorOpen == true)
	{
		FTLDoorOpening->Reverse();
		bIsDoorOpen = false;
	}

	FTLDoorOpening->TickTimeline(DeltaTime);
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

void UDoorTriggerComponent::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TotalMass += OtherActor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
}

void UDoorTriggerComponent::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	TotalMass -= OtherActor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
}


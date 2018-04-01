// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Classes/Engine/StaticMesh.h"
#include "ConstructorHelpers.h"

// Sets default values
ABaseProjectile::ABaseProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>("CollisionComp");
	CollisionComp->InitSphereRadius(10.0f);
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.0f));
	RootComponent = CollisionComp;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	
	VisibleComp = CreateDefaultSubobject<UStaticMeshComponent>("VisibleComponent");
	ConstructorHelpers::FObjectFinder<UStaticMesh> SMToSet(TEXT("/Game/Shape_Sphere"));
	if (SMToSet.Succeeded())
	{
		VisibleComp->SetStaticMesh(SMToSet.Object);
		VisibleComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		VisibleComp->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
		VisibleComp->SetVisibility(true);
		VisibleComp->SetMobility(EComponentMobility::Movable);
		VisibleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	

	


	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent");
	MovementComp->UpdatedComponent = RootComponent;
	MovementComp->bAutoActivate = false;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bShouldBounce = true;
	InitialLifeSpan = 4.0f;
}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseProjectile::Launch()
{
	MovementComp->SetVelocityInLocalSpace(FVector::ForwardVector * 1000.0f);
	MovementComp->Activate();
}



// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractiveMesh.h"
#include "Components/StaticMeshComponent.h"

AInteractiveMesh::AInteractiveMesh()
{
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->SetCollisionProfileName(FName("PhysicsActor"));
}

void AInteractiveMesh::InitializeMesh(EMeshType MeshType)
{
	CurrentType = MeshType;
	switch (MeshType)
	{
	case EMeshType::MT_None:
		break;
	case EMeshType::MT_WT_Semi:
		GetStaticMeshComponent()->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Meshes/Shape_Cube")));
		break;
	case EMeshType::MT_WT_Auto:
		GetStaticMeshComponent()->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Meshes/Shape_Cone")));
		break;
	}
	GetStaticMeshComponent()->RegisterComponent();
	SetActorScale3D(FVector(0.125f, 0.125f, 0.125f));
}

EMeshType AInteractiveMesh::GetMeshType()
{
	return CurrentType;
}

void AInteractiveMesh::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	InitializeMesh(CurrentType);
}



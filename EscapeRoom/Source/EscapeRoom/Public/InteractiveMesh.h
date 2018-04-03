// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "InteractiveMesh.generated.h"

UENUM()
enum class EMeshType : uint8
{
	MT_None,
	MT_WT_Semi,
	MT_WT_Auto
};


/**
 * 
 */
UCLASS()
class ESCAPEROOM_API AInteractiveMesh : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	AInteractiveMesh();

	void InitializeMesh(EMeshType MeshType);
	EMeshType GetMeshType();

private:

	UPROPERTY(EditAnywhere)
	EMeshType CurrentType = EMeshType::MT_None;

	virtual void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent) override;
	
};

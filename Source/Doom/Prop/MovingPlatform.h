// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SecretDoor.h"
#include "MovingPlatform.generated.h"


class UStaticMeshComponent;


UCLASS()
class DOOM_API AMovingPlatform : public ASecretDoor
{
	GENERATED_BODY()

public:

	AMovingPlatform();

protected:


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TArray<UStaticMeshComponent*> generatorLights;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* greenlightMaterial;

	

	int32 activatedGenerators = 0;



public:

	void activateGenerators();

	void interact(FString interactedComponentName, AActor* interactingActor) override;
};

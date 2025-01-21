// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "Generator.generated.h"

/**
 * 
 */
UCLASS()
class DOOM_API AGenerator : public AInteractable
{
	GENERATED_BODY()

public:

	AGenerator();


//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* generatorLight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* greenlightMaterial;


public:

	void interact(FString interactedComponentName, AActor* interactingActor) override;
	
};

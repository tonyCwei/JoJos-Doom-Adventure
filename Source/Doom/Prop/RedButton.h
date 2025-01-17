// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "RedButton.generated.h"

/**
 * 
 */
UCLASS()
class DOOM_API ARedButton : public AInteractable
{
	GENERATED_BODY()


public:

	ARedButton();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* buttonTop;

	bool canBePressed = true;

public:

	void interact(FString interactedComponentName, AActor* interactingActor) override;
	
};

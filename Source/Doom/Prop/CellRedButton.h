// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RedButton.h"
#include "CellRedButton.generated.h"

/**
 * 
 */
class ACellDoor;


UCLASS()
class DOOM_API ACellRedButton : public ARedButton
{
	GENERATED_BODY()


protected:
	
	void interact(FString interactedComponentName, AActor* interactingActor) override;

	bool isActived = false;

};

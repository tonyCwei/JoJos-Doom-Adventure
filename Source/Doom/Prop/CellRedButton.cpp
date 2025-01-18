// Fill out your copyright notice in the Description page of Project Settings.


#include "CellRedButton.h"
#include "CellDoor.h"
#include "EmergencyLight.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

void ACellRedButton::interact(FString interactedComponentName, AActor* interactingActor)
{
	Super::interact(interactedComponentName, interactingActor);

	

	if (!isActived) {
		isActived = true;

		TArray<AActor*> myCellDoors;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACellDoor::StaticClass(), myCellDoors);

		for (AActor* cellDoor : myCellDoors) {

			Cast<ACellDoor>(cellDoor)->open();

		}

		TArray<AActor*> myEmergencyLights;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEmergencyLight::StaticClass(), myEmergencyLights);

		for (AActor* lights : myEmergencyLights) {

			Cast<AEmergencyLight>(lights)->shutDown();

		}
	
	}

	
}

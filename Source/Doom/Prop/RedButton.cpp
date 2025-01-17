// Fill out your copyright notice in the Description page of Project Settings.


#include "RedButton.h"

ARedButton::ARedButton()
{
	PrimaryActorTick.bCanEverTick = true;

	buttonTop = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("buttonTop"));
	buttonTop->SetupAttachment(interactableMesh);
}

void ARedButton::interact(FString interactedComponentName, AActor* interactingActor)
{
	if (interactedComponentName == interactableMesh->GetName() && canBePressed) {
		canBePressed = false;
		buttonTop->SetRelativeLocation(FVector(0,0,-2));
		FTimerHandle resetTimerHandle;
		GetWorldTimerManager().SetTimer(resetTimerHandle, [&]()
			{
				buttonTop->SetRelativeLocation(FVector(0, 0, 0));
				canBePressed = true;
			}, 0.2, false);
	}
}

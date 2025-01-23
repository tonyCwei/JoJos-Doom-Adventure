// Fill out your copyright notice in the Description page of Project Settings.


#include "SecretDoor.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ASecretDoor::ASecretDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	movingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MovingMesh"));
	movingMesh->SetupAttachment(RootComponent);

	translateTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TranslateTimeline"));

	rotationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RotationTimeline"));
}

void ASecretDoor::BeginPlay()
{
	Super::BeginPlay();

	startingRelativeLocation = movingMesh->GetRelativeLocation();
	startingRelativeRotation = movingMesh->GetRelativeRotation();


	if (translateFloatCurve) {
		// Bind the update function
		FOnTimelineFloat translateUpdateFunction;
		translateUpdateFunction.BindUFunction(this, FName("translateTimelineUpdate"));

		// Bind the finished function
		FOnTimelineEvent translateFinishedFunction;
		translateFinishedFunction.BindUFunction(this, FName("translateTimelineFinished"));

		// Add functions to the timeline
		translateTimeline->AddInterpFloat(translateFloatCurve, translateUpdateFunction);
		translateTimeline->SetTimelineFinishedFunc(translateFinishedFunction);

		// Set the timeline to loop or play once
		translateTimeline->SetLooping(false);

	}

	if (rotationFloatCurve) {
		// Bind the update function
		FOnTimelineFloat rotationUpdateFunction;
		rotationUpdateFunction.BindUFunction(this, FName("rotationTimelineUpdate"));

		// Bind the finished function
		FOnTimelineEvent rotationFinishedFunction;
		rotationFinishedFunction.BindUFunction(this, FName("rotationTimelineFinished"));

		// Add functions to the timeline
		rotationTimeline->AddInterpFloat(rotationFloatCurve, rotationUpdateFunction);
		rotationTimeline->SetTimelineFinishedFunc(rotationFinishedFunction);

		// Set the timeline to loop or play once
		rotationTimeline->SetLooping(false);

	}
}

void ASecretDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASecretDoor::translateTimelineUpdate(float Value)
{
	FVector newLocation = FMath::Lerp(startingRelativeLocation, targetRelativeLocation, Value);
	movingMesh->SetRelativeLocation(newLocation);
}

void ASecretDoor::translateTimelineFinished()
{
	return;
}

void ASecretDoor::rotationTimelineUpdate(float Value)
{
	FRotator newRotation = FMath::Lerp(startingRelativeRotation, targetRelativeRotation, Value);
	movingMesh->SetRelativeRotation(newRotation);
}

void ASecretDoor::rotationTimelineFinished()
{
	return;
}



bool ASecretDoor::hasNeededKeys(AActor* interactingActor)
{
	for (FName key : keysNeeded) {
		if (!interactingActor->ActorHasTag(key)) {
			return false;
		}
	}

	return true;
}

void ASecretDoor::playMovingPlatformSound()
{
	if (MovingPlatformSound) {
		UGameplayStatics::PlaySoundAtLocation(this, MovingPlatformSound, movingMesh->GetComponentLocation());
	
	}
}

void ASecretDoor::interact(FString interactedComponentName, AActor* interactingActor)
{
	Super::interact(interactedComponentName, interactingActor);
	


	if (hasNeededKeys(interactingActor) && !isActivated) {
		isActivated = true;
		
		if (shouldTranslate)  translateTimeline->Play();
		if (shouldRotate)  rotationTimeline->Play();

		playSuccessSound();
		playMovingPlatformSound();
	}
	else if (!errorMessagePlayed && !isActivated) {
		errorMessagePlayed = true;
		playErrorSound();

		FTimerHandle errorMessageTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(errorMessageTimerHandle, [&]()
			{
				errorMessagePlayed = false;
			}, 6, false);



	}

}

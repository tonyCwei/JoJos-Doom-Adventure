// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
AInteractable::AInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	interactableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractableMesh"));
	interactableMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();
	this->Tags.Add(FName("Interactable"));
	interactableMesh->ComponentTags.Add(FName("InteractPrompt"));

}

//// Called every frame
//void AInteractable::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}


void AInteractable::playErrorSound()
{
	if (errorSound) {
		UGameplayStatics::PlaySoundAtLocation(this, errorSound, this->GetActorLocation());

	}
}

void AInteractable::playSuccessSound()
{
	if (successSound) {
		UGameplayStatics::PlaySoundAtLocation(this, successSound, this->GetActorLocation());

	}
}

void AInteractable::interact(FString interactedComponentName, AActor* interactingActor)
{
	if (interactedComponentName != interactableMesh->GetName()) return;
	
	//UE_LOG(LogTemp, Display, TEXT("%s is being interacted"), *this->GetName());

	
}



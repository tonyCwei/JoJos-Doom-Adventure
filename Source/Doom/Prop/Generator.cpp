// Fill out your copyright notice in the Description page of Project Settings.


#include "Generator.h"
#include "Components/StaticMeshComponent.h"
#include "MovingPlatform.h"
#include "Kismet/GameplayStatics.h"

AGenerator::AGenerator()
{
	PrimaryActorTick.bCanEverTick = true;

	generatorLight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GeneratorLight"));
	generatorLight->SetupAttachment(interactableMesh);
}

//void AGenerator::BeginPlay()
//{
//	Super::BeginPlay();
//}

void AGenerator::interact(FString interactedComponentName, AActor* interactingActor)
{
	
	Super::interact(interactedComponentName, interactingActor);
	
	if (!isActivated && greenlightMaterial) {
		isActivated = true;
		playSuccessSound();
		generatorLight->SetMaterial(0, greenlightMaterial);
		interactableMesh->ComponentTags.Empty();

		AMovingPlatform* myPlatform = Cast<AMovingPlatform>(UGameplayStatics::GetActorOfClass(GetWorld(), AMovingPlatform::StaticClass()));
		if (myPlatform) myPlatform->activateGenerators();
		
		
	}

}

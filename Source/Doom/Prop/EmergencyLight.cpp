// Fill out your copyright notice in the Description page of Project Settings.


#include "EmergencyLight.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AEmergencyLight::AEmergencyLight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	lightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("lightMesh"));
	RootComponent = lightMesh;


	rotatingLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("RotatingLight"));
	rotatingLight->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AEmergencyLight::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEmergencyLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (rotatingLight) {
		FRotator targetRotation = rotatingLight->GetRelativeRotation() + FRotator(0, 200, 0) * DeltaTime;
		rotatingLight->SetRelativeRotation(targetRotation);
	}
	


}

void AEmergencyLight::shutDown()
{
	rotatingLight->DestroyComponent();
}


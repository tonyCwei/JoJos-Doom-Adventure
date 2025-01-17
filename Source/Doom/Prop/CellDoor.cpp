// Fill out your copyright notice in the Description page of Project Settings.


#include "CellDoor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/RectLightComponent.h"
#include "Curves/CurveFloat.h"
#include "Components/TimelineComponent.h"


// Sets default values
ACellDoor::ACellDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	glassDoorBase =  CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GlassDoorBase"));
	RootComponent = glassDoorBase;

	glassDoorMove = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GlassDoorMove"));
	glassDoorMove->SetupAttachment(RootComponent);

	cellLight = CreateDefaultSubobject<URectLightComponent>(TEXT("CellLight"));
	cellLight -> SetupAttachment(RootComponent);
	

	doorOpenTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("CellDoorOpenTimeline"));

}

// Called when the game starts or when spawned
void ACellDoor::BeginPlay()
{
	Super::BeginPlay();

	glassDoorStartLocation = glassDoorMove->GetRelativeLocation();
	cellLight->SetVisibility(false);
	
	if (FloatCurve) {
		// Bind the update function
		FOnTimelineFloat UpdateFunction;
		UpdateFunction.BindUFunction(this, FName("doorOpenTimelineUpdate"));


		// Add functions to the timeline
		doorOpenTimeline->AddInterpFloat(FloatCurve, UpdateFunction);


		// Set the timeline to loop or play once
		doorOpenTimeline->SetLooping(false);

	}
}

// Called every frame
void ACellDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACellDoor::doorOpenTimelineUpdate(float Value)
{
	FVector curLocation = FMath::Lerp(glassDoorStartLocation, FVector(-192,67,0), Value);
	glassDoorMove->SetRelativeLocation(curLocation);
}

void ACellDoor::open()
{
	cellLight->SetVisibility(true);
	doorOpenTimeline->Play();
}


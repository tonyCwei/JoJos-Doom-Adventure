// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneratorPuzzle.h"

AGeneratorPuzzle::AGeneratorPuzzle()
{
	PrimaryActorTick.bCanEverTick = true;


	generator1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Generator1"));
	generator1->SetupAttachment(RootComponent);

	generator2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Generator2"));
	generator2->SetupAttachment(RootComponent);

	generator3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Generator3"));
	generator3->SetupAttachment(RootComponent);
}

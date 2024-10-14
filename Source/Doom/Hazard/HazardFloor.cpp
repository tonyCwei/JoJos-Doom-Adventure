// Fill out your copyright notice in the Description page of Project Settings.


#include "HazardFloor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Doom/DoomCharacter.h"


// Sets default values
AHazardFloor::AHazardFloor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	RootComponent = boxCollision;

	hazardCube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HazardCube"));
	hazardCube->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AHazardFloor::BeginPlay()
{
	Super::BeginPlay();
	
	boxCollision->OnComponentBeginOverlap.AddDynamic(this, &AHazardFloor::boxCollisionBeginOverlap);
	boxCollision->OnComponentEndOverlap.AddDynamic(this, &AHazardFloor::boxCollisionEndOverlap);
}

void AHazardFloor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearTimer(DamageEvent);
}

// Called every frame
void AHazardFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHazardFloor::boxCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player")) {
		damagedActor = OtherActor;

		GetWorldTimerManager().SetTimer(DamageEvent, [&]() { 
			DamageActor();
			}, damageRate, true, 0);
		
	}
	
		
}

void AHazardFloor::boxCollisionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("Player")) {
		GetWorldTimerManager().ClearTimer(DamageEvent);
	}
	
}

void AHazardFloor::DamageActor()
{
	if (damagedActor) {
		UGameplayStatics::ApplyDamage(damagedActor, damageAmount, nullptr, this, UDamageType::StaticClass());
	}
	
	
}


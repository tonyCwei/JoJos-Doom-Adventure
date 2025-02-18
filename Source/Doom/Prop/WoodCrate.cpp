// Fill out your copyright notice in the Description page of Project Settings.


#include "WoodCrate.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Doom/Item/ItemPickup.h"
#include "Components/BoxComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWoodCrate::AWoodCrate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	woodCrateGeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("woodCrateGeometryCollection"));
	RootComponent = woodCrateGeometryCollection;
	
	
	droppedItemPlace = CreateDefaultSubobject<USceneComponent>(TEXT("droppedItemPlace"));
	droppedItemPlace->SetupAttachment(RootComponent);
	
	
	

	boxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("XoxCollision"));
	boxCollision->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AWoodCrate::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add("WoodCrate");
}

void AWoodCrate::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearTimer(destroyTimerHandle);
}

void AWoodCrate::Destruct(FVector hitLocation)
{

	//UE_LOG(LogTemp, Display, TEXT("destruct"));

	if (destroySound) {
		UGameplayStatics::PlaySoundAtLocation(this, destroySound, this->GetActorLocation());
	}

	//Spawn item
	if (!droppedItemClasses.IsEmpty()) {
		int32 itemIndex = FMath::RandRange(0, droppedItemClasses.Num() - 1);
		TSubclassOf<AItemPickup> itemClass = droppedItemClasses[itemIndex];

		if (itemClass) {
			GetWorld()->SpawnActor<AItemPickup>(itemClass, droppedItemPlace->GetComponentLocation(), droppedItemPlace->GetComponentRotation());
		}
	}
	


	//Fracture
	if (woodCrateGeometryCollection && boxCollision) {
		woodCrateGeometryCollection->SetSimulatePhysics(true);
		TArray<float> emptyDamageThresholds;
		woodCrateGeometryCollection->SetDamageThreshold(emptyDamageThresholds);//DamageThresholds needed to make box not break on collision
		
		woodCrateGeometryCollection->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
		woodCrateGeometryCollection->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);//Make gun ignore fractured piece
		woodCrateGeometryCollection->AddRadialImpulse(hitLocation, 1, 200, ERadialImpulseFalloff::RIF_Constant, true);
		
		
		
		//woodCrateGeometryCollection->SetCollisionEnabled(ECollisionEnabled::);
		
		boxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		
	}

	
	GetWorld()->GetTimerManager().SetTimer(destroyTimerHandle, [&]()
		{
			Destroy();
		}, 2, false);
	
}




// Called every frame
//void AWoodCrate::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}




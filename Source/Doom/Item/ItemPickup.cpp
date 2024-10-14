// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemPickup.h"
#include "PaperSpriteComponent.h"
#include "Components/SphereComponent.h"
#include "Doom/DoomCharacter.h"

// Sets default values
AItemPickup::AItemPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	RootComponent = sphereCollision;

	itemSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("ItemSprite"));
	itemSprite->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AItemPickup::BeginPlay()
{
	Super::BeginPlay();
	
	sphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AItemPickup::BeginOverlap);
}

// Called every frame
void AItemPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotateImage(DeltaTime);

}

void AItemPickup::RotateImage(float DeltaTime)
{
	FRotator newRotator = GetActorRotation();
	newRotator.Yaw += DeltaTime * rotationSpeed;
	SetActorRotation(newRotator);
}

void AItemPickup::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor->ActorHasTag("Player")) {
		ADoomCharacter* playerCharacterRef = Cast<ADoomCharacter>(OtherActor);

		if (playerCharacterRef) {
			switch (itemType)
			{
			case Shield:
				playerCharacterRef->pickupShield(itemAmount);
				break;

			case Health:
				playerCharacterRef->pickupHealth(itemAmount);
				break;

			case BulletAmmo:
				playerCharacterRef->pickupAmmo(1, itemAmount);
				break;

			case ShellAmmo:
				playerCharacterRef->pickupAmmo(2, itemAmount);
				break;

			case RocketAmmo:
				playerCharacterRef->pickupAmmo(3, itemAmount);
				break;

			case CellAmmo:
				playerCharacterRef->pickupAmmo(4, itemAmount);
				break;

			case RedKeyCard:
				playerCharacterRef->pickupKey(1);
				break;

			case OrangeKeyCard:
				playerCharacterRef->pickupKey(2);
				break;

			case BlueKeyCard:
				playerCharacterRef->pickupKey(3);
				break;

			default:
				break;
			}

		}

		Destroy();
	
	}

}


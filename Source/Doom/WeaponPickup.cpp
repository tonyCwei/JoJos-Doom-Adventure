// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponPickup.h"
#include "PaperSpriteComponent.h"
#include "Components/SphereComponent.h"
#include "BaseWeapon.h"
#include "DoomCharacter.h"

// Sets default values
AWeaponPickup::AWeaponPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (!RootComponent) {
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	}

	sphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	sphereCollision->SetupAttachment(RootComponent);

	WeaponSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("WeaponSprite"));
	WeaponSprite->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AWeaponPickup::BeginPlay()
{
	Super::BeginPlay();
	sphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponPickup::BeginOverlap);
}

// Called every frame
void AWeaponPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotateImage(DeltaTime);

}

void AWeaponPickup::RotateImage(float DeltaTime) {
	FRotator newRotator = GetActorRotation();
	newRotator.Yaw += DeltaTime * rotationSpeed;
	SetActorRotation(newRotator);
}

void AWeaponPickup::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) {
	//UE_LOG(LogTemp, Warning, TEXT("overlap event"));

	if (OtherActor->ActorHasTag("Player")) {
		ADoomCharacter* playerCharacter = Cast<ADoomCharacter>(OtherActor);
		if (playerCharacter) {
			if (WeaponClass) {
				playerCharacter->pickupWeapon(WeaponClass);
				Destroy();
			}
		}
	}
	
}


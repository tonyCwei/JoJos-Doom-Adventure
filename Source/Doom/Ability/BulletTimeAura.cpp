// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletTimeAura.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h" 
#include "Sound/SoundCue.h"

// Sets default values
ABulletTimeAura::ABulletTimeAura()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	RootComponent = sphereCollision;

	SphereMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
	SphereMeshComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ABulletTimeAura::BeginPlay()
{
	Super::BeginPlay();
	UGameplayStatics::PlaySound2D(this, slowTimeStart);
	
	GetWorld()->GetTimerManager().SetTimer(destroyHandle, [&]()
		{
			UGameplayStatics::PlaySound2D(this, slowTimeEnd);
			Destroy();
		}, duration, false);

	sphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ABulletTimeAura::BeginOverlap);
	sphereCollision->OnComponentEndOverlap.AddDynamic(this, &ABulletTimeAura::EndOverlap);

}

void ABulletTimeAura::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearTimer(destroyHandle);
}

// Called every frame
void ABulletTimeAura::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABulletTimeAura::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {
		if (!OtherActor->ActorHasTag("Player")) {
			OtherActor->CustomTimeDilation = slowDownRate;
		}
	}
}

void ABulletTimeAura::EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor) {
		if (!OtherActor->ActorHasTag("Player")) {
			OtherActor->CustomTimeDilation = 1;
		}
	}
}


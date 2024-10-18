// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseDoor.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "Components/TimelineComponent.h"
#include "Doom/DoomCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


// Sets default values
ABaseDoor::ABaseDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	door = CreateDefaultSubobject<USceneComponent>(TEXT("door"));
	door->SetupAttachment(RootComponent);

	doorSmallUI = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorSmallUI"));
	doorSmallUI->SetupAttachment(door);

	steamDoor = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SteamDoor"));
	steamDoor->SetupAttachment(RootComponent); 

	steamDoor2 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SteamDoor2"));
	steamDoor2->SetupAttachment(RootComponent);

	doorOpenTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorOpenTimeline"));


}

// Called when the game starts or when spawned
void ABaseDoor::BeginPlay()
{
	Super::BeginPlay();

	if (isLocked) {
		doorSmallUI->SetMaterial(0, lockedDoorUIMaterial);
	}
	else {
		doorSmallUI->SetMaterial(0, unlockedDoorUIMaterial);
	}

	if (FloatCurve) {
		// Bind the update function
		FOnTimelineFloat UpdateFunction;
		UpdateFunction.BindUFunction(this, FName("doorOpenTimelineUpdate"));

		// Bind the finished function
		FOnTimelineEvent FinishedFunction;
		FinishedFunction.BindUFunction(this, FName("doorOpenTimelineFinished"));

		// Add functions to the timeline
		doorOpenTimeline->AddInterpFloat(FloatCurve, UpdateFunction);
		doorOpenTimeline->SetTimelineFinishedFunc(FinishedFunction);

		// Set the timeline to loop or play once
		doorOpenTimeline->SetLooping(false);
	
	}
	
}

void ABaseDoor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearTimer(closeTimerHanlde);
}

// Called every frame
void ABaseDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseDoor::doorOpenTimelineUpdate(float Value)
{
	FVector newLocation = FMath::Lerp(doorClosed, doorOpen, Value);
	door->SetRelativeLocation(newLocation);
}

void ABaseDoor::doorOpenTimelineFinished()
{


	if (!isClosed) {
		GetWorldTimerManager().SetTimer
		(closeTimerHanlde, [&]() {
			doorOpenTimeline->Reverse();
			isClosed = true;
		}, 
		1, false);
	}
}

void ABaseDoor::openDoor()
{

	if (isClosed && !isLocked) {
		isClosed = false;

		steamDoor->SetActive(true);
		steamDoor2->SetActive(true);

		doorOpenTimeline->Play();
	}
	
}

void ABaseDoor::unlockDoor()
{
	//Gray Door
	if (keyColor == "GrayKey") {
		//Do Dead Door
		//call player ui and update message
		UE_LOG(LogTemp, Display, TEXT("Dead Door"));
		return;
	} 
	
	//Player No Key
	ADoomCharacter* playerCharacterRef = Cast<ADoomCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	
	if (!playerCharacterRef->ActorHasTag(keyColor)) {
		//call player ui and update message
		UE_LOG(LogTemp, Display, TEXT("You need to aquire %s !"), *keyColor.ToString());

		if (lockedSound) {
			UGameplayStatics::PlaySoundAtLocation(this, lockedSound, this->GetActorLocation());
		}

		return;
	}

	//Unlcok
	isLocked = false;

	if (unlockedDoorUIMaterial) {
		doorSmallUI->SetMaterial(0, unlockedDoorUIMaterial);
	}

	if (unlockSound) {
		UGameplayStatics::PlaySoundAtLocation(this, unlockSound, this->GetActorLocation());
	}
	
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "BGMManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABGMManager::ABGMManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	BGMAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BGMAudioComponent"));
	BGMAudioComponent->bAutoActivate = false;
	RootComponent = BGMAudioComponent;

}

// Called when the game starts or when spawned
void ABGMManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABGMManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABGMManager::PlayBGM(USoundBase* NewBGM, float FadeInDuration, float Volume)
{
    if (!NewBGM || !BGMAudioComponent) return;

    if (BGMAudioComponent->IsPlaying())
    {
        // Fade out the current BGM before changing
        BGMAudioComponent->FadeOut(FadeInDuration, 0.0f);
    }

    CurrentBGM = NewBGM;
    

    // Play the new BGM
    BGMAudioComponent->SetSound(NewBGM);
    BGMAudioComponent->FadeIn(FadeInDuration, Volume);
}

void ABGMManager::StopBGM(float FadeOutDuration)
{
    if (BGMAudioComponent && BGMAudioComponent->IsPlaying())
    {
        BGMAudioComponent->FadeOut(FadeOutDuration, 0.0f);
    }
    CurrentBGM = nullptr;
}

void ABGMManager::PauseBGM()
{
    if (BGMAudioComponent && BGMAudioComponent->IsPlaying())
    {
        BGMAudioComponent->SetPaused(true);
    }
}

void ABGMManager::ResumeBGM()
{
    if (BGMAudioComponent && BGMAudioComponent->bIsPaused)
    {
        BGMAudioComponent->SetPaused(false);
    }
}

void ABGMManager::setVolume(float newVolume)
{
    BGMAudioComponent->SetVolumeMultiplier(newVolume);
}


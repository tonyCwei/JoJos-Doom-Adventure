// Fill out your copyright notice in the Description page of Project Settings.


#include "Chainsaw.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"


AChainsaw::AChainsaw()
{
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
}



void AChainsaw::FireWeapon()
{
	Super::FireWeapon();
	if (activeSound) {
		AudioComponent->SetSound(activeSound);
	}
}






void AChainsaw::StopFire()
{
	Super::StopFire();
	if (standbySound) {
		AudioComponent->SetSound(standbySound);
	}
}

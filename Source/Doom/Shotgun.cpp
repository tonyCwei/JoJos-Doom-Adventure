// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"

void AShotgun::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    GetWorldTimerManager().ClearTimer(ShotgunTimerHandle);
}

void AShotgun::FireWeapon() {
    if (bFireOnce) {
        bFireOnce = false;
        Super::FireWeapon();
        resetFlipbook();

        //control fire rate
        
	    GetWorld()->GetTimerManager().SetTimer(ShotgunTimerHandle, [&]()
	    {
	    bFireOnce = true;
	    }, fireRate, false);  
    } 
}
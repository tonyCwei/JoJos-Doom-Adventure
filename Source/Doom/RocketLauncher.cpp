// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketLauncher.h"

void ARocketLauncher::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    GetWorldTimerManager().ClearTimer(RocketLauncherTimerHandle);
}

void ARocketLauncher::FireWeapon() {
    if (bFireOnce) {
        bFireOnce = false;
        ShootProjectle();
        resetFlipbook();

        //control fire rate
       
	    GetWorld()->GetTimerManager().SetTimer(RocketLauncherTimerHandle, [&]()
	    {
	    bFireOnce = true;
	    }, fireRate, false);  
    } 
}
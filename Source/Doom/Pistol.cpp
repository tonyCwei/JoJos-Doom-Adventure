// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"

void APistol::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    GetWorldTimerManager().ClearTimer(PistolTimerHandle);
}

void APistol::FireWeapon() {
    if (bFireOnce) {
        bFireOnce = false;
        Super::FireWeapon();
        resetFlipbook();

        //control fire rate
        
        GetWorld()->GetTimerManager().SetTimer(PistolTimerHandle, [&]()
        {
                bFireOnce = true;
        }, fireRate, false);
    } 
}
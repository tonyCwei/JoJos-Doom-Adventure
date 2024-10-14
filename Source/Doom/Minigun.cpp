// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigun.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"
#include "BaseWeapon.h"



void AMinigun::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    GetWorldTimerManager().ClearTimer(minigunEvent);
}

void AMinigun::FireWeapon() {
    GetWorld()->GetTimerManager().SetTimer(minigunEvent, this, &AMinigun::CallFire, fireRate, true, 0);
}


void AMinigun::CallFire() {
    Super::FireWeapon();
    if (!hasEnoughAmmo()) StopFire();
}


void AMinigun::StopFire() {
    GetWorldTimerManager().ClearTimer(minigunEvent);
    resetFlipbook();
}
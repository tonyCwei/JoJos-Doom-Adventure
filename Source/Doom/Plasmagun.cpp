// Fill out your copyright notice in the Description page of Project Settings.


#include "Plasmagun.h"



//Plasmagun shoots projectle instead of line trace
void APlasmagun::CallFire() {
    ShootProjectle();
    if (!hasEnoughAmmo()) StopFire();
}
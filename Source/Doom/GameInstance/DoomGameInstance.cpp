// Fill out your copyright notice in the Description page of Project Settings.


#include "DoomGameInstance.h"
#include "DoomSaveGame.h"
#include "Kismet/GameplayStatics.h"

UDoomGameInstance::UDoomGameInstance()
{
    doomSaveGame = Cast<UDoomSaveGame>(UGameplayStatics::LoadGameFromSlot("SavingsSlot", 0));
    if (!doomSaveGame)
    {
        doomSaveGame = Cast<UDoomSaveGame>(UGameplayStatics::CreateSaveGameObject(UDoomSaveGame::StaticClass()));
        UGameplayStatics::SaveGameToSlot(doomSaveGame, "SavingsSlot", 0);
    }

    
}

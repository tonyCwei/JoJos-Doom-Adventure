// Fill out your copyright notice in the Description page of Project Settings.


#include "DoomGameInstance.h"
#include "DoomSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Doom/DoomCharacter.h"

UDoomGameInstance::UDoomGameInstance()
{
    doomSaveGame = Cast<UDoomSaveGame>(UGameplayStatics::LoadGameFromSlot("SavingsSlot", 0));
    if (!doomSaveGame)
    {
        doomSaveGame = Cast<UDoomSaveGame>(UGameplayStatics::CreateSaveGameObject(UDoomSaveGame::StaticClass()));
        UGameplayStatics::SaveGameToSlot(doomSaveGame, "SavingsSlot", 0);
    }

    
}

void UDoomGameInstance::SavePlayerData()
{
    ACharacter* playerCharacterRef = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (playerCharacterRef) {
        ADoomCharacter* doomCharacterRef = Cast<ADoomCharacter>(playerCharacterRef);
        if (doomCharacterRef) {
            playerHealth = doomCharacterRef->getCurHealth();
            playerShield = doomCharacterRef->getCurShield();
            playerBullet = doomCharacterRef->getBullet();
            playerShell = doomCharacterRef->getShell();
            playerCell = doomCharacterRef->getCell();
            playerRocekt = doomCharacterRef->getRocket();

        }

    }

}

void UDoomGameInstance::LoadPlayerData()
{
    ACharacter* playerCharacterRef = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (playerCharacterRef) {
        ADoomCharacter* doomCharacterRef = Cast<ADoomCharacter>(playerCharacterRef);
        if (doomCharacterRef) {
            doomCharacterRef->setCurHealth(playerHealth);
            doomCharacterRef->setCurShield(playerShield);
            doomCharacterRef->bullet = playerBullet;
            doomCharacterRef->shell = playerShell;
            doomCharacterRef->cell = playerCell;
            doomCharacterRef->rocket = playerRocekt;

        }

    }
}

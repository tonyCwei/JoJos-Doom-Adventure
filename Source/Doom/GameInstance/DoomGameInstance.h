// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DoomGameInstance.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class DOOM_API UDoomGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UDoomGameInstance();


protected:



	float playerHealth = 100;

	float playerShield = 100;

	int32 playerBullet = 50;

	int32 playerShell = 0;

	int32 playerCell = 0;

	int32 playerRocekt = 0;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDoomSaveGame* doomSaveGame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float countDownSeconds;

	//PlayerData
	UFUNCTION(BlueprintCallable)
	void SavePlayerData();

	UFUNCTION(BlueprintCallable)
	void LoadPlayerData();
};

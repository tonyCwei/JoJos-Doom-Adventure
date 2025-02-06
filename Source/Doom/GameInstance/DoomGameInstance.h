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

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDoomSaveGame* doomSaveGame;
};

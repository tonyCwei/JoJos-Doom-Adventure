// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DoomSaveGame.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class DOOM_API UDoomSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

//Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	bool isFullScreen = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString graphicsSelection = "High";


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float mouseSensitivity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float zoomSensitivity = 0.5f;


	UFUNCTION(BlueprintCallable)
	void applySettings();
};

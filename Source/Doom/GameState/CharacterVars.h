// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterVars.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class DOOM_API UCharacterVars : public UDataAsset
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float sensitivityX = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float sensitivityY = 0.5;
};

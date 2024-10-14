// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigun.h"
#include "Plasmagun.generated.h"

/**
 * 
 */
UCLASS()
class DOOM_API APlasmagun : public AMinigun
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void CallFire() override;


};

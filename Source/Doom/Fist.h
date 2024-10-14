// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "Fist.generated.h"

/**
 * 
 */
UCLASS()
class DOOM_API AFist : public ABaseWeapon
{
	GENERATED_BODY()


private:
	FTimerHandle MeleeTimerHandle;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:

	void FireWeapon() override;

	void Punch();

	
};

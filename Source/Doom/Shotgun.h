// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class DOOM_API AShotgun : public ABaseWeapon
{
	GENERATED_BODY()

private:
	FTimerHandle ShotgunTimerHandle;

	bool bFireOnce = true;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:

	void FireWeapon() override;
	
};

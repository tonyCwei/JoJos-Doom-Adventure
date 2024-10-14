// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "Pistol.generated.h"

/**
 * 
 */
UCLASS()
class DOOM_API APistol : public ABaseWeapon
{
	GENERATED_BODY()

private:
	
	bool bFireOnce = true;

	FTimerHandle PistolTimerHandle;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:

	void FireWeapon() override;
};

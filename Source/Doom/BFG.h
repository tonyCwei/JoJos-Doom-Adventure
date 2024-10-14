// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "BFG.generated.h"

/**
 * 
 */
UCLASS()
class DOOM_API ABFG : public ABaseWeapon
{
	GENERATED_BODY()

private:
	
	bool bFireOnce = true;

	FTimerHandle BFGFireHandle;

	FTimerHandle BFGTimerHandle;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:

	void FireWeapon() override;
	
	void ShootProjectle() override;
};

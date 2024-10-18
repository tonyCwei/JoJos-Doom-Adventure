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


public:

	ABFG();

private:
	
	bool hasFired = false;

	bool isCharging = false;

	FTimerHandle BFGFireHandle;

	FTimerHandle BFGTimerHandle;

	FTimerHandle BFGFlipbookHandle;

	FTimerHandle BFGAudioHandle;

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float chargeTimeNeeded;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flipbooks", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* ChargingFlipbook;

	float chargeStartTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* myAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	class USoundCue* chargingSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	class USoundCue* fullychargedSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	class USoundCue* restSound;

public:

	void FireWeapon() override;
	
	void ShootProjectle() override;

	void StopFire() override;

	void cancelCharging();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigun.h"
#include "Chainsaw.generated.h"

/**
 * 
 */
UCLASS()
class DOOM_API AChainsaw : public AMinigun
{
	GENERATED_BODY()
	

public:
	AChainsaw();

protected:



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* AudioComponent;



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	class USoundCue* standbySound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	class USoundCue* activeSound;




	void FireWeapon() override;

	void StopFire() override;
};

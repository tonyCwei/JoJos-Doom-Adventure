// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DoomGameStateBase.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FAttackInfo
{
	GENERATED_BODY()

	float StartTime;

	float Duration;

	AActor* Attacker;

	bool operator==(const FAttackInfo& Other) const
	{
		return (StartTime == Other.StartTime &&
			Duration == Other.Duration &&
			Attacker == Other.Attacker);
	}
};



UCLASS()
class DOOM_API ADoomGameStateBase : public AGameStateBase
{
	GENERATED_BODY()



	//Attack infos for perfect dodge

private:
	//perfect dodges
	TArray<FAttackInfo> activeAttacks;

public:

	UFUNCTION(BlueprintCallable)
	void addAttack(FAttackInfo curAttackInfo);

	UFUNCTION(BlueprintCallable)
	void removeAttack(FAttackInfo curAttackInfo);

	UFUNCTION(BlueprintCallable)
	int getActiveAttacksSize() { return activeAttacks.Num(); }

	UFUNCTION(BlueprintCallable)
	TArray<FAttackInfo> getActiveAttacks() { return activeAttacks; }
	
};

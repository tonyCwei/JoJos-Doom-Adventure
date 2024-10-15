// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "SelfDestructEnemy.generated.h"

/**
 * 
 */
UCLASS()
class DOOM_API ASelfDestructEnemy : public ABaseEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASelfDestructEnemy();


protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);


	bool isSelfDes = false;

	bool isDestructing = false;

	
	//Blink effects
	bool isRed = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	float blinkRate = 0.3;

	//TimerHandles
	FTimerHandle selfDesTimerHandle;

	FTimerHandle sphereTraceTimerHandle;

	FTimerHandle blinkEffectTimerHandle;

	FTimerHandle resetCanSeePlayerTimerHandle;

	//Self Des
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	float destructRadius = 450;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	float destructDamage = 20;

	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	float destructTime = 3; //blinking time

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	float destructDodgeWindow = 0.3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flipbooks", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* selfDesFlipbook;


	//Pawn sensing and BT in C++
	class AAIController* myAIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	class UPawnSensingComponent* PawnSensingComponent;

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	void setBlackBoardCanSeePlayer(bool value);

	/*UFUNCTION()
	void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* myBehaviorTree;

public:
	UFUNCTION(BlueprintCallable)
	void selfDestruct();

	void HandleDeath() override;

	
};

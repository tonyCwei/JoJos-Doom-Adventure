// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "Harubus.generated.h"




class UTimelineComponent;

/**
 * 
 */
UCLASS()
class DOOM_API AHarubus : public ABaseEnemy
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	AHarubus();
	
protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	// Called every frame
	void Tick(float DeltaTime) override;




//Aditional General
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flipbooks", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* AnimationFlipBookComponent;


//Utilities
protected:

	void facePlayerYaw();

	void disableGravity();

	void enableGravity();

	void setCollisionOverlapPawn();

	void setCollisionBlockPawn();

//120 degree Attack

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OnetwentyAttack", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ProjectileSpawn2;

	FTimerHandle onetwentyAttackTimer;

	UTimelineComponent* onetwentyTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OnetwentyAttack", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* onetwentyFloatCurve;

	UFUNCTION()
	void onetwentyTimelineUpdate(float Value);

	UFUNCTION()
	void onetwentyTimelineFinished();

	FRotator onetwentyStart;

	FRotator onetwentyEnd;

public:

	void ShootProjectle() override;


	UFUNCTION(BlueprintCallable)
	void onetwentyAttack();





// Drop Attack

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropAttack", meta = (AllowPrivateAccess = "true"))
	UPaperFlipbook* preDropFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropAttack", meta = (AllowPrivateAccess = "true"))
	float dropAttackDelay = 3;

	void startPreDropAttack();

	void startDropAttack();

	UTimelineComponent* dropAttackTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropAttack", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* dropAttackFloatCurve;

	UFUNCTION()
	void dropAttackTimelineUpdate(float Value);

	UFUNCTION()
	void dropAttackTimelineFinished();

	FVector dropAttackStartLocation;
	
	FVector dropAttackEndLocation;

	void dropAttackSphereTrace();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropAttack", meta = (AllowPrivateAccess = "true"))
	float dropAttackRange = 500;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropAttack", meta = (AllowPrivateAccess = "true"))
	float dropAttackDamage = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropAttack", meta = (AllowPrivateAccess = "true"))
	float dropAttackIndicatorTime = 1.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropAttack", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AActor> dropAttackIndicator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropAttack", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AActor> dropAttackLandEffect;


public:


	UFUNCTION(BlueprintCallable)
	void dropAttack();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaserTower.generated.h"

class UNiagaraComponent;

UCLASS()
class DOOM_API ALaserTower : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaserTower();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* tower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* laser1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* laser1VFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* laser2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* laser2VFX;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* towerActivateTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* towerRiseFloatCurve;

	UFUNCTION()
	void towerActivateTimelineUpdate(float Value);

	UFUNCTION()
	void towerActivateTimelineFinished();

	FVector deactivatedLocation;

	FVector activatedLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	float riseDistance = 500;

	
//Tower Laser	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	float laserLength = 3000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	float laserDamage = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	float laserHurtInterval = 0.2;

	bool isLaserActive = false;

	bool canLaserHurt = true;

	void laserHasHurt();

	FTimerHandle laserHurtTimerHandle;

	void laserTick();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	float towerRotationSpeed = 100;

	bool shouldTowerRotate = false;
	
	void rotateTower(float DeltaTime);

	bool isTowerActivation = false;


	UFUNCTION(BlueprintCallable)
	void activateLaser();

	UFUNCTION(BlueprintCallable)
	void deactivateLaser();


public:

	UFUNCTION(BlueprintCallable)
	void activateTower();

	UFUNCTION(BlueprintCallable)
	void deactivateTower();

//Audio
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BGM")
	class UAudioComponent* laserAudio1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BGM")
	class UAudioComponent* laserAudio2;
	
};

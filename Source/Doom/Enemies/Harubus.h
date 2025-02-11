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

	bool shouldFacePlayer = false;


//BossHUD
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UBossHUD> bossHUDClass;

	class UBossHUD* myBossHUD;

public:

	UFUNCTION(BlueprintCallable)
	void addBossHUD();

	void updateHealthBar();

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


// Summon Attack
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SummonAttack", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ASelfDestructEnemy> SelfDesEnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SummonAttack", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AActor> SelfDesEnemyFlameClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SummonAttack", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AActor> summonAttackEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SummonAttack", meta = (AllowPrivateAccess = "true"))
	float SpawnInterval = 0.25;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SummonAttack", meta = (AllowPrivateAccess = "true"))
	USceneComponent* SummonSpawn0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SummonAttack", meta = (AllowPrivateAccess = "true"))
	USceneComponent* SummonSpawn1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SummonAttack", meta = (AllowPrivateAccess = "true"))
	USceneComponent* SummonSpawn2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SummonAttack", meta = (AllowPrivateAccess = "true"))
	USceneComponent* SummonSpawn3;

	TArray<USceneComponent*> summonSpawns;

	FTimerHandle spawnTimerHandle;

	int32 spawnCount = 0;

	UFUNCTION()
	void spawnSelfDes();


	
public:



	UFUNCTION(BlueprintCallable)
	void summonAttack();

	UFUNCTION()
	void startSummonAttack();


//Laser Attack
protected:
	//Self Laser
	FTimerHandle startLaserAttackTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LaserAttack", meta = (AllowPrivateAccess = "true"))
	USceneComponent* laserSpwan;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LaserAttack", meta = (AllowPrivateAccess = "true"))
	USceneComponent* preLaserEffectSpwan;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LaserAttack", meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* laserVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LaserAttack", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AActor> preLaserAttackEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LaserAttack", meta = (AllowPrivateAccess = "true"))
	UPaperFlipbook* laserAttackFlipbook;

	bool isLaserAttacking = false;

	void laserAttackTick();
	
	bool canLaserHurt = true;

	void laserHasHurt();

	FTimerHandle laserHurtTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LaserAttack", meta = (AllowPrivateAccess = "true"))
	float laserHurtInterval = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LaserAttack", meta = (AllowPrivateAccess = "true"))
	float laserDamage = 1;


	//Laser Tower
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LaserAttack", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ALaserTower> laserTowerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LaserAttack", meta = (AllowPrivateAccess = "true"))
	FVector laserTowerSpwanLocation;

	class ALaserTower* laserTowerRef;


public:

	UFUNCTION(BlueprintCallable)
	void laserAttack();

	UFUNCTION(BlueprintCallable)
	void endLaserAttack();


//Rising Sun Attack
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RisingSunAttack", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AActor> risingProjectileClass;


	FVector risingSunSpawnLocation;
public:
	UFUNCTION(BlueprintCallable)
	void risingSunAttack();

//Death

protected:
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flipbooks", meta = (AllowPrivateAccess = "true"))
	TArray<class UPaperFlipbook*> deathFlipbooks;*/

	void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* DamageInstigator, AActor* DamageCauser) override;

	void HandleDeath() override;

	void HandleBossDeath();

	void playBossEndSequence();


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> TVStaticWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	class ULevelSequence* bossEndSequence;

	UFUNCTION()
	void OnBossEndSequenceFinished();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	class USoundBase* BossEndSong;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* PromptChildActorComponent;

	
public:
	void executeBoss();

//Audio
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* myAudioComponent;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* preDropSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* midDropSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* finalDropSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* summonSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* laserStartSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* sunAttackSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* laserAttackSound;
};

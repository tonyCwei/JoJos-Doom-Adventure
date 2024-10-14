// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Doom/GameState/DoomGameStateBase.h"
#include "BaseEnemy.generated.h"


UENUM(BlueprintType)
enum EnemyState {
	IdleState UMETA(DisplayName = "Idle"),
	MovingState UMETA(DisplayName = "Moving"),
	PreMeleeAttackState UMETA(DisplayName = "PreMeleeAttacking"),
	MeleeAttackState UMETA(DisplayName = "MeleeAttacking"),
	RangedAttackState UMETA(DisplayName = "RangedAttacking")
};


UENUM(BlueprintType)
enum AttackingState {
	PreMeleeAttacking UMETA(DisplayName = "PreMeleeAttackState"),
	MeleeAttacking UMETA(DisplayName = "MeleeAttackState"),
	RangedAttacking UMETA(DisplayName = "RangedAttackState")
};



UCLASS()
class DOOM_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	
	//General
protected:
	//Player Reference
	class ADoomCharacter* playerCharacter;

	ADoomGameStateBase* gameStateRef;

	bool canSeePlayer = false;

	//TimerHandles
	FTimerHandle attackingTimerHandle;

	FTimerHandle deathTimerHandle;

	FTimerHandle attackWindowTimerHandle;

public:
	UFUNCTION(BlueprintCallable)
	bool getCanSeePlayer() { return canSeePlayer; }

	UFUNCTION(BlueprintCallable)
	void setCanSeePlayer(bool canSeePlayer_) { canSeePlayer = canSeePlayer_; }

	UFUNCTION(BlueprintCallable)
	class AAIController* getAIController();


	//Flipbook
protected: 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Flipbooks", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* EnemyFlipBookComponent;


	void updateDirectionalSprite();

	void updateFlipbook(float degree, int32 index);

	void rotateToPlayer(float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float rotationSpeed = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flipbooks", meta = (AllowPrivateAccess = "true"))
	TArray<class UPaperFlipbook*> directionalFlipbooks;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flipbooks", meta = (AllowPrivateAccess = "true"))
	TArray<class UPaperFlipbook*> movingFlipbooks;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flipbooks", meta = (AllowPrivateAccess = "true"))
	TArray<class UPaperFlipbook*> meleeAttackFlipbooks;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flipbooks", meta = (AllowPrivateAccess = "true"))
	TArray<class UPaperFlipbook*> preMeleeAttackFlipbooks;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flipbooks", meta = (AllowPrivateAccess = "true"))
	TArray<class UPaperFlipbook*> rangedAttackFlipbooks;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flipbooks", meta = (AllowPrivateAccess = "true"))
	TArray<class UPaperFlipbook*> currentFlipbooks;

	
	UFUNCTION(BlueprintCallable)
	void CheckEnemyState();

	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flipbooks", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EnemyState> enemyState = IdleState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flipbooks", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<AttackingState> attackingstate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool isAttacking = false;
	
	//Ranged Attack
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ABaseProjectile> ProjectileClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ProjectileSpawn;




public:

	UFUNCTION(BlueprintCallable)
	virtual void ShootProjectle();



// Melee Attack
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float meleeDamage = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float meleeAttackWindow = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float meleeAttackRange = 150;

	FAttackInfo curAttackInfo;

	bool isAdded;

public:

	UFUNCTION(BlueprintCallable)
	virtual void MeleeAttack();


// Damage System
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float curHealth = 5;

	UFUNCTION()
	void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* DamageInstigator, AActor* DamageCauser);

//Death

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flipbooks", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* deathFlipbook;

	UFUNCTION(BlueprintCallable)
	void HandleDeath();

	bool isDead = false;

	bool isDying = false;

};

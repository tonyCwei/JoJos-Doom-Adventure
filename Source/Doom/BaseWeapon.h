// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BaseWeapon.generated.h"

UENUM(BlueprintType)
enum Ammo {
	Bullet UMETA(DisplayName = "Bullet"),
	Shell UMETA(DisplayName = "Shell"),
	Rocket UMETA(DisplayName = "Rocket"),
	Cell UMETA(DisplayName = "Cell"),
	MeleeWeapon UMETA(DisplayName = "MeleeWeapon")
};


UCLASS()
class DOOM_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


//TimerHandles

private:
	FTimerHandle resetFlipbookTimerHandle;


protected:
	//Player Reference
	class ADoomCharacter* playerCharacter;

	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ArrowComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* LineTraceComponent;

	//Weapon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flipbooks", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* ShootingFlipbook;

	class UPaperFlipbook* IdleFlipbook;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flipbooks", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* WeaponFlipBookComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float lineTraceDistance = 4000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float weaponDamage = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float fireRate = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<Ammo> ammoType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ABaseProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> bloodToSpawn;

public:

	UFUNCTION(BlueprintCallable)
	virtual void FireWeapon();

	UFUNCTION(BlueprintCallable)
	virtual void ShootProjectle();

	UFUNCTION(BlueprintCallable)
	virtual void StopFire();

	// UFUNCTION(BlueprintCallable)
	// virtual void Punch();

	UFUNCTION(BlueprintCallable)
	virtual void PlayFireAnimation();

	UFUNCTION(BlueprintCallable)
	void resetFlipbook();

	UFUNCTION(BlueprintCallable)
	bool hasEnoughAmmo();

	UFUNCTION(BlueprintCallable)
	void decreaseAmmo();

	UFUNCTION()
	Ammo GetAmmoType() { return ammoType; }

};

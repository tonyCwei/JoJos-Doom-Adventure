// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "DoomCharacter.generated.h"


class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class USpotLightComponent;

class UTimelineComponent;
class USoundCue;


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);




UCLASS(config=Game)
class ADoomCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	// UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	// USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* ShootAction;

	/** Melee Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MeleeAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	/** ToggleFlash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleFlashAction;
	
public:
	ADoomCharacter();

protected:
	virtual void BeginPlay();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
		
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for shooting input */
	void Shoot(const FInputActionValue& Value);

	void StopShoot(const FInputActionValue& Value);

	void Jump() override;

	void ToggleFlash(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	//USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

//Game State Ref
protected:
	class ADoomGameStateBase* gameStateRef;



private:
	//FlashLights
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	USpotLightComponent* outerFlashLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	USpotLightComponent* innerFlashLight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* flashToggleSound;


	//Weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* WeaponChildActorComponent;	

	FVector WeaponChildActorLocation;
	
	UPROPERTY(VisibleAnywhere)
	class ABaseWeapon* mainWeapon;


	//Melee
	/** Called for melee input */
	void Melee(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABaseWeapon> fistClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABaseWeapon> curWeaponClass;

	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee", meta = (AllowPrivateAccess = "true"))
	float meleeRate = 0.5f;

	bool canMelee = true;

	bool isShooting = false;

//TimerHandles
private:
	FTimerHandle MeleeHandle;

	FTimerHandle regenTimerHandle;

	FTimerHandle sprintTimerHandle;

	FTimerHandle ShootMeleeHandle;

	FTimerHandle dashTimerHandle;

	FTimerHandle perfectDodgeTimerHandle;

	FTimerHandle perfectDodgeEffectHandle;

	FTimerHandle scanTimerHandle;

public:
	UFUNCTION(BlueprintCallable)
	void ClearAllTimerHandles();

//Different Types of Bullets
	//Pistol
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	int32 bullet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	int32 maxBullet = 200;
	
	//Shotgun
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	int32 shell;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	int32 maxShell = 50;

	//Rocket Launcher
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	int32 rocket;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	int32 maxRocket = 50;
	
	//Laser
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	int32 cell;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	int32 maxCell = 300;

//Player HUD
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UPlayerHUD> playerHUDClass;

	class UPlayerHUD* playerHUD;

	void UpdateCurAmmoText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> DamagedHUDClass;
	
	
	

public:
	//Getter
	UFUNCTION(BlueprintCallable)
	int32 getBullet() const {return bullet;}

	UFUNCTION(BlueprintCallable)
	int32 getShell() const {return shell;}

	UFUNCTION(BlueprintCallable)
	int32 getRocket() const {return rocket;}

	UFUNCTION(BlueprintCallable)
	int32 getCell() const {return cell;}

	UFUNCTION(BlueprintCallable)
	int32 getMaxBullet() const {return maxBullet;}

	UFUNCTION(BlueprintCallable)
	int32 getMaxShell() const {return maxShell;}

	UFUNCTION(BlueprintCallable)
	int32 getMaxRocket() const {return maxRocket;}

	UFUNCTION(BlueprintCallable)
	int32 getMaxCell() const {return maxCell;}

	UFUNCTION()
	UPlayerHUD* getPlayerHUD() { return playerHUD; }

	UFUNCTION(BlueprintCallable)
	bool getCanMelee() const {return canMelee;}

	//DecreaseAmmmo
	UFUNCTION(BlueprintCallable)
	void decreaseBullet() { bullet--; }

	UFUNCTION(BlueprintCallable)
	void decreaseShell() { shell--; }

	UFUNCTION(BlueprintCallable)
	void decreaseRocket() { rocket--; }

	UFUNCTION(BlueprintCallable)
	void decreaseCell() { cell--; }

	UFUNCTION(BlueprintCallable)
	void setCell(int32 newCell) { cell = newCell; }

	UFUNCTION(BlueprintCallable)
	void setCanMelee(bool newVal)  {canMelee = newVal;}




	//Weapon Pickup
	UFUNCTION()
	void pickupWeapon(TSubclassOf<ABaseWeapon> WeaponClass);



	//Movement
	UFUNCTION(BlueprintCallable)
	bool isMoving() const;

//Sprint
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 1000;

	void SprintStart(const FInputActionValue& Value);

	void SprintEnd(const FInputActionValue& Value);

	void drainStamina();

	void regenStamina();

	

public:
	bool isSprinting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats", meta = (AllowPrivateAccess = "true"))
	float stamina = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats", meta = (AllowPrivateAccess = "true"))
	float maxStamina = 100;

//Dash and Dodge
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dodge", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ABulletTimeAura> myBulletTimeAura;

	bool isDashing = false;

	bool isInvincible = false;

	float dashCD = 1;

	UFUNCTION()
	void Dash(const FInputActionValue& Value);

	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dodge", meta = (AllowPrivateAccess = "true"))
	float perfectDodgeWindow = 0.5;

public:	

	UFUNCTION(BlueprintCallable)
	bool getIsInvincible() const { return isInvincible; }

	//Perfect Dodge
	UFUNCTION(BlueprintCallable)
	void checkPerfectDodge();

	UFUNCTION(BlueprintCallable)
	void perfectDodge();





//Weapon Bob
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponBob", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* WeaponBobMovementCurveH;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponBob", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* WeaponBobMovementCurveV;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponBob", meta = (AllowPrivateAccess = "true"))
	bool ShouldBob = true;

	UTimelineComponent* WeaponBobTimeline;

	

	UFUNCTION()
	void WeaponBobTimelineProgress();

	void WeaponBobTimelineUpdate(float HValue, float VValue);

	UFUNCTION()
	void WeaponBob(float DeltaTime);


	// UFUNCTION()
	// void WeaponBobTimelineFinished();

//Weapon Swap
private:
	//Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Weapon1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Weapon2Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Weapon3Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Weapon4Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Weapon5Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Weapon6Action;

	void SwapWeapon1(const FInputActionValue& Value);

	void SwapWeapon2(const FInputActionValue& Value);

	void SwapWeapon3(const FInputActionValue& Value);

	void SwapWeapon4(const FInputActionValue& Value);

	void SwapWeapon5(const FInputActionValue& Value);

	void SwapWeapon6(const FInputActionValue& Value);

	/*End of Input Setup*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponSwap", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<ABaseWeapon>>  AllWeapons;
	
	int32 SwapIndex = 0;
	
	bool IsSwapping = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponSwap", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* WeaponSwapCurve;

	UTimelineComponent* WeaponSwapTimeline;

	UFUNCTION()
	void WeaponSwapTimelineProgress(float Alpha);

	UFUNCTION()
	void WeaponSwapTimelineFinished();

	UTimelineComponent* WeaponSwapResetTimeline;

	UFUNCTION()
	void WeaponSwapResetTimelineProgress(float Alpha);

	UFUNCTION()
	void WeaponSwapResetTimelineFinished();


public:

	UFUNCTION(BlueprintCallable)
	void WeaponSwap(int32 WeaponIndex);


	//Health and Shield

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats", meta = (AllowPrivateAccess = "true"))
	float curHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats", meta = (AllowPrivateAccess = "true"))
	float maxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats", meta = (AllowPrivateAccess = "true"))
	float curShield = 15;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats", meta = (AllowPrivateAccess = "true"))
	float maxShield = 100;

	//Take Damage
	UFUNCTION()
	void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* DamageInstigator, AActor* DamageCauser);

//Death
protected:
	bool isAlive = true;

	UFUNCTION(BlueprintCallable)
	void HandleDeath();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UTimelineComponent* deathTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* deathFloatCurve;

	UFUNCTION()
	void deathTimelineUpdate(float Value);

	UFUNCTION()
	void deathTimelineFinished();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> GameOverHUDClass;

public:
	UFUNCTION(BlueprintCallable)
	bool getIsAlive() { return isAlive; }

public:
	//Setter and Getter
	UFUNCTION(BlueprintCallable)
	float getCurHealth() const { return curHealth; }

	UFUNCTION(BlueprintCallable)
	float getMaxHealth() const { return maxHealth; }

	UFUNCTION(BlueprintCallable)
	float getCurShield() const { return curShield; }

	UFUNCTION(BlueprintCallable)
	float getMaxShield() const { return maxShield; }

	UFUNCTION(BlueprintCallable)
	void setCurHealth(float curHealth_) { curHealth = curHealth_; }

	UFUNCTION(BlueprintCallable)
	void setMaxHealth(float maxHealth_) { maxHealth = maxHealth_; }

	UFUNCTION(BlueprintCallable)
	void setCurShield(float curShield_) { curShield = curShield_; }

	UFUNCTION(BlueprintCallable)
	void setMaxShield(float maxShield_) { maxShield = maxShield_; }


	//Item Pickup
public:
	UFUNCTION(BlueprintCallable)
	void pickupShield(float shieldAmount);

	UFUNCTION(BlueprintCallable)
	void pickupHealth(float HealthAmount);

	UFUNCTION(BlueprintCallable)
	void pickupAmmo(int32 ammoIndex, int32 amount);
	
	//Interact Button
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats", meta = (AllowPrivateAccess = "true"))
	float interactDistance = 100;


	UFUNCTION(BlueprintCallable)
	void Interact(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void InteractDoor(AActor* door);

	//KeyCard System

public:
	UFUNCTION(BlueprintCallable)
	void pickupKey(int32 colorIndex);

	//Camera Zoom
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ZoomAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* zoomFloatCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UTimelineComponent* zoomTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	float zoomWalkSpeed = 300;


	UFUNCTION()
	void zoomTimelineUpdate(float Value);

	bool isZooming = false;

	bool canZoom();

	void ZoomIn(const FInputActionValue& Value);

	void ZoomOut(const FInputActionValue& Value);

	void ZoomOutOverride();

public:

	bool getIsZooming() const { return isZooming; }

//Scanner
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ScanAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scan", meta = (AllowPrivateAccess = "true"))
	TSubclassOf< class AScanner> ScannerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scan", meta = (AllowPrivateAccess = "true"))
	float scanCD = 5;

	bool canScan = true;

	void Scan();

//Noise Emitter for AI
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UPawnNoiseEmitterComponent* myNoiseEmitter;

//Audio

	UFUNCTION()
	void play2DSound(USoundCue* soundCue);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* takeDamageSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* deathSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* jumpSound;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* zoomSound;

	//update crosshair
protected:
	void updateCrosshair();
};


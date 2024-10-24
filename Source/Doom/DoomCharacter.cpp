// Copyright Epic Games, Inc. All Rights Reserved.

#include "DoomCharacter.h"
#include "Animation/AnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "UI/PlayerHUD.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Components/ChildActorComponent.h"
#include "BaseWeapon.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Ability/BulletTimeAura.h"
#include "Doom/GameState/DoomGameStateBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Prop/BaseDoor.h"
#include "Ability/Scanner.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Sound/SoundCue.h"






DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ADoomCharacter

ADoomCharacter::ADoomCharacter()
{


	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;


	//Create Weapon Child Actor
	WeaponChildActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("WeaponChildActor"));
	WeaponChildActorComponent->SetupAttachment(FirstPersonCameraComponent);

	//Create Weapon Bob Timeline
	WeaponBobTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("WeaponBobTimeline"));
	
	//Create Weapon Swap Timeline
	WeaponSwapTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("WeaponSwapTimeline"));
	WeaponSwapResetTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("WeaponSwapResetTimeline"));

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	// Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	// Mesh1P->SetOnlyOwnerSee(true);
	// Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	// Mesh1P->bCastDynamicShadow = false;
	// Mesh1P->CastShadow = false;
	// //Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	// Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	//Create Death Timeline
	deathTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DeathTimeline"));

	//Create Zoom Timeline
	zoomTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ZoomTimeline"));

	//Noise Emitter
	myNoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitter"));
	

}

void ADoomCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		//Avoid inital click after click play in viewport
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
	}

	//Game State Ref
	gameStateRef = GetWorld()->GetGameState<ADoomGameStateBase>();



	//Assign Main Weapon
	mainWeapon = Cast<ABaseWeapon>(WeaponChildActorComponent->GetChildActor());
	WeaponChildActorLocation = WeaponChildActorComponent->GetRelativeLocation();

	//Player HUD
	if (playerHUDClass) {
		playerHUD = CreateWidget<UPlayerHUD>(this->GetWorld(), playerHUDClass);
		if (playerHUD) {
			playerHUD->AddToViewport();
		}
	}

	UpdateCurAmmoText();

	/*Weapon Bob Timeline binding*/
	
	//Handle Update pin in timeline blueprint, binding multiple curves to a single timeline
	FOnTimelineFloat WeaponBobMovementValueH;
	FOnTimelineFloat WeaponBobMovementValueV;

	//Event for Update pin 
	FOnTimelineEvent WeaponBobTimelineTickEvent;

	WeaponBobTimelineTickEvent.BindUFunction(this, FName("WeaponbobTimelineProgress"));

	if (WeaponBobMovementCurveH && WeaponBobMovementCurveV) {
		WeaponBobTimeline->AddInterpFloat(WeaponBobMovementCurveH, WeaponBobMovementValueH);
		WeaponBobTimeline->AddInterpFloat(WeaponBobMovementCurveV, WeaponBobMovementValueV);
	}
	WeaponBobTimeline->SetTimelinePostUpdateFunc(WeaponBobTimelineTickEvent);
	WeaponBobTimeline->SetTimelineLength(2);
	WeaponBobTimeline->SetLooping(true);
	

	//Handle Finished pin in timeline blueprint
	//FOnTimelineEvent WeaponBobTimelineFinishedEvent;
	//WeaponBobTimelineFinishedEvent.BindUFunction(this, FName("WeaponbobTimelineFinished"));
	//WeaponBobTimeline->SetTimelineFinishedFunc(WeaponBobTimelineFinishedEvent);

	/*Weapon Swap Timeline Binding*/
	WeaponSwapTimeline->SetTimelineLength(0.5);
	WeaponSwapResetTimeline->SetTimelineLength(0.5);


	FOnTimelineFloat WeaponSwapMovementValue;
	WeaponSwapMovementValue.BindUFunction(this, FName("WeaponSwapTimelineProgress"));
	if (WeaponSwapCurve) {
		WeaponSwapTimeline->AddInterpFloat(WeaponSwapCurve, WeaponSwapMovementValue);
	}
	FOnTimelineEvent WeaponSwapTimelineFinishedEvent;
	WeaponSwapTimelineFinishedEvent.BindUFunction(this, FName("WeaponSwapTimelineFinished"));
	WeaponSwapTimeline->SetTimelineFinishedFunc(WeaponSwapTimelineFinishedEvent);

	FOnTimelineFloat WeaponSwapResetMovementValue;
	WeaponSwapResetMovementValue.BindUFunction(this, FName("WeaponSwapResetTimelineProgress"));
	if (WeaponSwapCurve) {
		WeaponSwapResetTimeline->AddInterpFloat(WeaponSwapCurve, WeaponSwapResetMovementValue);
	}
	FOnTimelineEvent WeaponSwapResetTimelineFinishedEvent;
	WeaponSwapResetTimelineFinishedEvent.BindUFunction(this, FName("WeaponSwapResetTimelineFinished"));
	WeaponSwapResetTimeline->SetTimelineFinishedFunc(WeaponSwapResetTimelineFinishedEvent);


	//Sprint
	WalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	//Bind anyDamage
	OnTakeAnyDamage.AddDynamic(this, &ADoomCharacter::DamageTaken);

	//Death Timeline Binding
	if (deathFloatCurve) {
		// Bind the update function
		FOnTimelineFloat deathUpdateFunction;
		deathUpdateFunction.BindUFunction(this, FName("deathTimelineUpdate"));

		// Bind the finished function
		FOnTimelineEvent deathFinishedFunction;
		deathFinishedFunction.BindUFunction(this, FName("deathTimelineFinished"));

		// Add functions to the timeline
		deathTimeline->AddInterpFloat(deathFloatCurve, deathUpdateFunction);
		deathTimeline->SetTimelineFinishedFunc(deathFinishedFunction);

		// Set the timeline to loop or play once
		deathTimeline->SetLooping(false);

	}

	//Zoom Timeline Binding
	if (zoomFloatCurve) {
		FOnTimelineFloat UpdateFunction;
		UpdateFunction.BindUFunction(this, FName("zoomTimelineUpdate"));
		zoomTimeline->AddInterpFloat(zoomFloatCurve, UpdateFunction);
	
	}

	
	
}

void ADoomCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ClearAllTimerHandles();

}

void ADoomCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!isAlive) return;
		
	WeaponBob(DeltaTime);
}

//////////////////////////////////////////////////////////////////////////// Input

void ADoomCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADoomCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADoomCharacter::Look);

		//Shooting
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ADoomCharacter::Shoot);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &ADoomCharacter::StopShoot);

		//Melee
		EnhancedInputComponent->BindAction(MeleeAction, ETriggerEvent::Started, this, &ADoomCharacter::Melee);

		//Weapon Swap
		EnhancedInputComponent->BindAction(Weapon1Action, ETriggerEvent::Started, this, &ADoomCharacter::SwapWeapon1);
		EnhancedInputComponent->BindAction(Weapon2Action, ETriggerEvent::Started, this, &ADoomCharacter::SwapWeapon2);
		EnhancedInputComponent->BindAction(Weapon3Action, ETriggerEvent::Started, this, &ADoomCharacter::SwapWeapon3);
		EnhancedInputComponent->BindAction(Weapon4Action, ETriggerEvent::Started, this, &ADoomCharacter::SwapWeapon4);
		EnhancedInputComponent->BindAction(Weapon5Action, ETriggerEvent::Started, this, &ADoomCharacter::SwapWeapon5);
		EnhancedInputComponent->BindAction(Weapon6Action, ETriggerEvent::Started, this, &ADoomCharacter::SwapWeapon6);

		//Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ADoomCharacter::SprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ADoomCharacter::SprintEnd);

		//Dash
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ADoomCharacter::Dash);

		//Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ADoomCharacter::Interact);

		//Zoom
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Started, this, &ADoomCharacter::ZoomIn);
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Completed, this, &ADoomCharacter::ZoomOut);

		//Scan
		EnhancedInputComponent->BindAction(ScanAction, ETriggerEvent::Started, this, &ADoomCharacter::Scan);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void ADoomCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ADoomCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ADoomCharacter::Shoot(const FInputActionValue& Value) {
	if (!canMelee || IsSwapping) return;

	mainWeapon->FireWeapon();
	
	myNoiseEmitter->MakeNoise(this, 1, GetActorLocation());

	isShooting = true;
	ShouldBob = false;
	WeaponChildActorComponent->SetRelativeLocation(WeaponChildActorLocation);

	
	GetWorld()->GetTimerManager().SetTimer(ShootMeleeHandle, [&]()
	{
		isShooting = false;
	}, 0.5, false);

}

void ADoomCharacter::StopShoot(const FInputActionValue& Value) {
	
	mainWeapon->StopFire();
	ShouldBob = true;
}

void ADoomCharacter::Jump()
{
	if (GetCharacterMovement()->IsMovingOnGround()) {
		Super::Jump();
		play2DSound(jumpSound);
	}
	
}

void ADoomCharacter::Melee(const FInputActionValue& Value) {
	if (!canMelee || isShooting || IsSwapping || isZooming) return;

	canMelee = false;
	curWeaponClass = WeaponChildActorComponent->GetChildActorClass();

	if (fistClass) {
		WeaponChildActorComponent->SetChildActorClass(fistClass);
		mainWeapon = Cast<ABaseWeapon>(WeaponChildActorComponent->GetChildActor());
		mainWeapon->FireWeapon();
	}

	
	GetWorld()->GetTimerManager().SetTimer(MeleeHandle, [&]()
	{
		if (curWeaponClass) {
			WeaponChildActorComponent->SetChildActorClass(curWeaponClass);	
			mainWeapon = Cast<ABaseWeapon>(WeaponChildActorComponent->GetChildActor());
			canMelee = true;
		}
	}, meleeRate, false);
}

void ADoomCharacter::ClearAllTimerHandles()
{
	GetWorldTimerManager().ClearTimer(MeleeHandle);
	GetWorldTimerManager().ClearTimer(regenTimerHandle);
	GetWorldTimerManager().ClearTimer(sprintTimerHandle);
	GetWorldTimerManager().ClearTimer(ShootMeleeHandle);
	GetWorldTimerManager().ClearTimer(dashTimerHandle);
	GetWorldTimerManager().ClearTimer(perfectDodgeTimerHandle);
	GetWorldTimerManager().ClearTimer(perfectDodgeEffectHandle);
	GetWorldTimerManager().ClearTimer(scanTimerHandle);
}


void ADoomCharacter::UpdateCurAmmoText()
{
	switch (mainWeapon->GetAmmoType()) {
		case Bullet:
			playerHUD->UpdateBullet();
			break;

		case Shell:
			playerHUD->UpdateShell();
			break;

		case Rocket:
			playerHUD->UpdateRocket();
			break;

		case Cell:
			playerHUD->UpdateCell();
			break;

		case MeleeWeapon:
			playerHUD->UpdateChainsaw();
			break;

		default:
			return;
	}



}

void ADoomCharacter::pickupWeapon(TSubclassOf<ABaseWeapon> WeaponClass) {
	//WeaponChildActorComponent->SetChildActorClass(WeaponClass);
	AllWeapons.Add(WeaponClass);
	//mainWeapon = Cast<ABaseWeapon>(WeaponChildActorComponent->GetChildActor());
	//UpdateCurAmmoText();

	//Swap to Picked up weapon
	WeaponSwap(AllWeapons.Num()-1);


	GetWorldTimerManager().ClearTimer(MeleeHandle);
	canMelee = true;
}

bool ADoomCharacter::isMoving() const
{
	return GetVelocity().Size() > 0;
}

void ADoomCharacter::SprintStart(const FInputActionValue& Value)
{
	isSprinting = true;
	ZoomOutOverride();
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	drainStamina();
}

void ADoomCharacter::SprintEnd(const FInputActionValue& Value)
{
	isSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	
	GetWorldTimerManager().ClearTimer(regenTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(regenTimerHandle, [&]()
		{
			regenStamina();
		}, 1, false);
}

void ADoomCharacter::drainStamina()
{
	stamina -= 0.5;
	if (stamina <= 0) stamina = 0;

	if (stamina == 0) {
		SprintEnd(1);
	}
	else if (isSprinting) {
		
		GetWorldTimerManager().SetTimer(sprintTimerHandle, [&]()
				{
					drainStamina();
				}, 0.01, false);
		
		
	}
}

void ADoomCharacter::regenStamina()
{
	if (isSprinting || isDashing) return;
	
	stamina += 1;
	if (stamina >= maxStamina) stamina = maxStamina;

	if (stamina < maxStamina) {
		
		GetWorldTimerManager().SetTimer(sprintTimerHandle, [&]()
				{
					regenStamina();
				}, 0.01, false);
		
		
	}
	
}



void ADoomCharacter::Dash(const FInputActionValue& Value)
{
	if (isDashing || stamina < 25 ) return;

	//ZoomOutOverride();

	isInvincible = true;
	isDashing = true;

	stamina -= 25;
	if (stamina <= 0) stamina = 0;


	//If using base velocity without normalize, dash will not work while character switching to oppsite direction
	FVector LaunchVelocity;
	
	
	if (isMoving()) {
		FVector movingVelocity = GetVelocity();
		movingVelocity.Normalize();
		LaunchVelocity = FVector(movingVelocity.X * 5000, movingVelocity.Y * 5000, 0);
	}
	else {
		FVector playerForward = this->GetActorForwardVector();
		LaunchVelocity = FVector(playerForward.X * -5000, playerForward.Y * -5000, 0);
	}
	
	
	LaunchCharacter(LaunchVelocity, false, false);

	GetWorld()->GetTimerManager().SetTimer(perfectDodgeTimerHandle, [&]()
		{
			isInvincible = false;
		}, perfectDodgeWindow, false);

	
	GetWorld()->GetTimerManager().SetTimer(dashTimerHandle, [&]()
		{
			isDashing = false;
		}, 0.8, false);

	GetWorldTimerManager().ClearTimer(regenTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(regenTimerHandle, [&]()
		{
			regenStamina();
		}, 1, false);

	checkPerfectDodge();

}

void ADoomCharacter::checkPerfectDodge()
{
	for (FAttackInfo& attackInfo : gameStateRef->getActiveAttacks()) {
		
		float timeTillHit = attackInfo.StartTime + attackInfo.Duration - GetWorld()->GetTimeSeconds();

		UE_LOG(LogTemp, Display, TEXT("TimeTillHit: %f"), timeTillHit);
		if (timeTillHit > 0 && timeTillHit <= perfectDodgeWindow) {
			perfectDodge();
		}
	
	}
}

void ADoomCharacter::perfectDodge()
{
	FVector spawnLocation = GetActorLocation();
	FRotator spawnRotation = GetActorRotation();
	//FTransform SpawnTransform = LineTraceComponent->GetComponentTransform();

	if (myBulletTimeAura) {
		GetWorld()->SpawnActor<ABulletTimeAura>(myBulletTimeAura, spawnLocation, spawnRotation);
	}

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);

	GetWorld()->GetTimerManager().SetTimer(perfectDodgeEffectHandle, [&]()
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		}, 0.05, false);
		
}



//Weapon Bob
void ADoomCharacter::WeaponBobTimelineProgress(){
	WeaponBobTimelineUpdate(
		WeaponBobMovementCurveH->GetFloatValue(WeaponBobTimeline->GetPlaybackPosition()),
		WeaponBobMovementCurveV->GetFloatValue(WeaponBobTimeline->GetPlaybackPosition())
	);

}

void ADoomCharacter::WeaponBobTimelineUpdate(float HValue, float VValue)
{

	float curX = WeaponChildActorComponent->GetRelativeLocation().X;

	//Horizontal bobing range
	float YAValue = WeaponChildActorComponent->GetRelativeLocation().Y + 0.04;
	float YBValue = WeaponChildActorComponent->GetRelativeLocation().Y - 0.04;
	float newY = FMath::Lerp(YAValue, YBValue, HValue);
	//Vetical Bobing range
	float ZAValue = WeaponChildActorComponent->GetRelativeLocation().Z + 0.01;
	float ZBValue = WeaponChildActorComponent->GetRelativeLocation().Z - 0.01;
	float newZ = FMath::Lerp(ZAValue, ZBValue, VValue);

	WeaponChildActorComponent->SetRelativeLocation(FVector(curX, newY, newZ));
}



void ADoomCharacter::WeaponBob(float DeltaTime)
{
	if (IsSwapping) return;


	if (isMoving() && ShouldBob ) {
		WeaponBobTimeline->Play();
	}
	else {
		WeaponBobTimeline->Stop();
		FVector newWeaponLocation = FMath::VInterpConstantTo(
			WeaponChildActorComponent->GetRelativeLocation(),
			WeaponChildActorLocation,
			DeltaTime,
			25
		);
		WeaponChildActorComponent->SetRelativeLocation(newWeaponLocation);
		
	}
	
}


//Weapon Swap
void ADoomCharacter::WeaponSwap(int32 WeaponIndex)
{
	ZoomOutOverride();
	//bool SameWeapon = AllWeapons[WeaponIndex] == WeaponChildActorComponent->GetChildActorClass();
	bool NoWeapon = WeaponIndex > AllWeapons.Num() - 1;
	if ( NoWeapon ||
		IsSwapping ||
		AllWeapons[WeaponIndex] == WeaponChildActorComponent->GetChildActorClass()//Same Weapon
		) 
	{
		//UE_LOG(LogTemp, Display, TEXT("No Weapon"));
		return;
	}

	//Stop fire cur weapon
	ABaseWeapon* curWeapon = Cast<ABaseWeapon>(WeaponChildActorComponent->GetChildActor());

	if (curWeapon) {
		curWeapon->StopFire();
	}


	IsSwapping = true;
	WeaponSwapTimeline->PlayFromStart();
	SwapIndex = WeaponIndex;

	
}





void ADoomCharacter::SwapWeapon1(const FInputActionValue& Value)
{
	WeaponSwap(1);
}

void ADoomCharacter::SwapWeapon2(const FInputActionValue& Value)
{
	WeaponSwap(2);
}

void ADoomCharacter::SwapWeapon3(const FInputActionValue& Value)
{
	WeaponSwap(3);
}

void ADoomCharacter::SwapWeapon4(const FInputActionValue& Value)
{
	WeaponSwap(4);
}

void ADoomCharacter::SwapWeapon5(const FInputActionValue& Value)
{
	WeaponSwap(5);
}

void ADoomCharacter::SwapWeapon6(const FInputActionValue& Value)
{
	WeaponSwap(6);
}

void ADoomCharacter::WeaponSwapTimelineProgress(float Alpha)
{
	float curX = WeaponChildActorComponent->GetRelativeLocation().X;
	float curY = WeaponChildActorComponent->GetRelativeLocation().Y;
	
	float ZAValue = WeaponChildActorComponent->GetRelativeLocation().Z;
	float ZBValue = WeaponChildActorComponent->GetRelativeLocation().Z - 0.8;
	float newZ = FMath::Lerp(ZAValue, ZBValue, Alpha);

	WeaponChildActorComponent->SetRelativeLocation(FVector(curX, curY, newZ));

}

void ADoomCharacter::WeaponSwapTimelineFinished()
{
	/*UE_LOG(LogTemp, Display, TEXT("WeaponSwapTimelineFinished"));*/
	WeaponChildActorComponent->SetChildActorClass(AllWeapons[SwapIndex]);
	mainWeapon = Cast<ABaseWeapon>(WeaponChildActorComponent->GetChildActor());
	UpdateCurAmmoText();

	//Play Weapon Swap sound when finishing swapping
	ABaseWeapon* curWeapon = Cast<ABaseWeapon>(WeaponChildActorComponent->GetChildActor());

	if (curWeapon) {
		curWeapon->playWeaponSwapSound();
	}


	WeaponSwapResetTimeline->PlayFromStart();


	
}

void ADoomCharacter::WeaponSwapResetTimelineProgress(float Alpha)
{
	float curX = WeaponChildActorComponent->GetRelativeLocation().X;
	float curY = WeaponChildActorComponent->GetRelativeLocation().Y;

	float ZAValue = WeaponChildActorComponent->GetRelativeLocation().Z;
	float ZBValue = WeaponChildActorLocation.Z;
	float newZ = FMath::Lerp(ZAValue, ZBValue, Alpha);

	WeaponChildActorComponent->SetRelativeLocation(FVector(curX, curY, newZ));
}

void ADoomCharacter::WeaponSwapResetTimelineFinished()
{
	IsSwapping = false;
	

}


void ADoomCharacter::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* DamageInstigator, AActor* DamageCauser)
{
	//needs sound
	if (isInvincible || !isAlive) return;

	if (curShield > 0) {
		curShield -= Damage;
		if (curShield <= 0) {
			curShield = 0;
		}
	}
	else {
		curHealth -= Damage;
	}

	

	if (curHealth <= 0) {
		curHealth = 0;
		HandleDeath();
	}

	playerHUD->UpdateCurrentHealth();
	playerHUD->UpdateCurrentShield();


	if (DamagedHUDClass) {
		UUserWidget* DamagedHUD = CreateWidget<UUserWidget>(this->GetWorld(), DamagedHUDClass);
		if (DamagedHUD) {
			DamagedHUD->AddToViewport();
		}
	}

	play2DSound(takeDamageSound);

}

void ADoomCharacter::HandleDeath()
{
	//needs sound

	isAlive = false;

	DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	GetCharacterMovement()->StopMovementImmediately();
	LaunchCharacter(FVector(GetActorForwardVector().X * -3000, 0, 300), false, false);

	deathTimeline->Play();

	play2DSound(deathSound);

	FTimerHandle cursorTimerHandle;

	GetWorld()->GetTimerManager().SetTimer(cursorTimerHandle, [&]()
		{

			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

			// Show the mouse cursor
			PlayerController->bShowMouseCursor = true;

			// Enable click events and mouse over events if needed
			PlayerController->bEnableClickEvents = true;
			PlayerController->bEnableMouseOverEvents = true;

			// Set the input mode to Game and UI to make sure the mouse is active immediately
			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputMode);

		}, 3, false);

	


}

void ADoomCharacter::deathTimelineUpdate(float Value)
{


	FVector camLocation = FMath::Lerp(FVector(-10,0,60), FVector(-10, 0, -60), Value);
	FirstPersonCameraComponent->SetRelativeLocation(camLocation);


	FVector weaponLocation = FMath::Lerp(FVector(36, 0, -11), FVector(36, 0, -100), Value);
	WeaponChildActorComponent->SetRelativeLocation(weaponLocation);
}

void ADoomCharacter::deathTimelineFinished()
{
	
	if (GameOverHUDClass) {
		UUserWidget* GameOverHUD = CreateWidget<UUserWidget>(this->GetWorld(), GameOverHUDClass);
		if (GameOverHUD) {
			GameOverHUD->AddToViewport();
		}
	
	
	}

}

void ADoomCharacter::pickupShield(float shieldAmount)
{
	curShield += shieldAmount;
	if (curShield >= maxShield) curShield = maxShield;

	playerHUD->UpdateCurrentShield();
}

void ADoomCharacter::pickupHealth(float healthAmount)
{
	curHealth += healthAmount;
	if (curHealth >= maxHealth) curHealth = maxHealth;

	playerHUD->UpdateCurrentHealth();
}

void ADoomCharacter::pickupAmmo(int32 ammoIndex, int32 ammoAmount)
{
	switch (ammoIndex)
	{	
		case 1:
			bullet += ammoAmount;
			if (bullet >= maxBullet) bullet = maxBullet;

			if (mainWeapon->GetAmmoType() == Bullet) {
				playerHUD->UpdateBullet();
			}
			else {
				playerHUD->UpdateBulletRight();
			}
			break;
		
		case 2:
			shell += ammoAmount;
			if (shell >= maxShell) shell = maxShell;

			if (mainWeapon->GetAmmoType() == Shell) {
				playerHUD->UpdateShell();
			}
			else {
				playerHUD->UpdateShellRight();
			}
			break;

		case 3:
			rocket += ammoAmount;
			if (rocket >= maxRocket) rocket = maxRocket;

			if (mainWeapon->GetAmmoType() == Rocket) {
				playerHUD->UpdateRocket();
			}
			else {
				playerHUD->UpdateRocketRight();
			}
			break;
		
		
		case 4:
			cell += ammoAmount;
			if (cell >= maxCell) cell = maxCell;

			if (mainWeapon->GetAmmoType() == Cell) {
				playerHUD->UpdateCell();
			}
			else {
				playerHUD->UpdateCellRight();
			}
			break;
		
		default:
			break;
	}
}

void ADoomCharacter::Interact(const FInputActionValue& Value)
{
	FVector lineTraceLocation = WeaponChildActorComponent->GetComponentLocation();
	FVector lineTraceForward = UKismetMathLibrary::GetForwardVector(WeaponChildActorComponent->GetComponentRotation());
	FVector lineTraceEnd = lineTraceForward * interactDistance + lineTraceLocation;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_WorldStatic),
														 UEngineTypes::ConvertToObjectType(ECC_WorldDynamic),
														 UEngineTypes::ConvertToObjectType(ECC_Pawn) };

	TArray<AActor*> ActorsToIgnore = { Cast<AActor>(this), UGameplayStatics::GetPlayerCharacter(this,0) };
	FHitResult HitResult;

	//Line Trace
	bool hasHit = UKismetSystemLibrary::LineTraceSingleForObjects(this->GetWorld(),
		lineTraceLocation,
		lineTraceEnd,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::Type::None,
		HitResult,
		true);

	if (hasHit) {
		AActor* HitActor = HitResult.GetActor();
		if (HitActor->ActorHasTag("Door")) {
			InteractDoor(HitActor);
		}

	}
}

void ADoomCharacter::InteractDoor(AActor* door)
{
	ABaseDoor* myDoor = Cast<ABaseDoor>(door);
	if (myDoor) {
		if (myDoor->getIsLocked()) {
			myDoor->unlockDoor();
	
		}
		else {
			myDoor->openDoor();
		}

	}


}

void ADoomCharacter::pickupKey(int32 colorIndex)
{
	switch (colorIndex)
	{
	case 1:
		Tags.Add(FName("RedKey"));
		playerHUD->ActiveRedKey();
		break;
	
	case 2:
		Tags.Add(FName("OrangeKey"));
		playerHUD->ActiveOrangeKey();
		break;
	
	case 3:
		Tags.Add(FName("BlueKey"));
		playerHUD->ActiveBlueKey();
		break;

	default:
		break;
	}


}

bool ADoomCharacter::canZoom()
{
	if (mainWeapon->GetAmmoType() == MeleeWeapon || isSprinting) return false;
	return true;
}

void ADoomCharacter::zoomTimelineUpdate(float Value)
{
	//lerp FOV for camera
	//float curFOV = FMath::Lerp(90, 60, Value);
	FirstPersonCameraComponent->SetFieldOfView(Value);
}

void ADoomCharacter::ZoomIn(const FInputActionValue& Value)
{
	if (canZoom()) {
		
		isZooming = true;
		zoomTimeline->Play();
		GetCharacterMovement()->MaxWalkSpeed = zoomWalkSpeed;
	}
}

void ADoomCharacter::ZoomOut(const FInputActionValue& Value)
{
	if (canZoom()) {

		isZooming = false;
		zoomTimeline->Reverse();
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	}
}

void ADoomCharacter::ZoomOutOverride()
{
	if (isZooming) {
		isZooming = false;
		zoomTimeline->Reverse();
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void ADoomCharacter::Scan()
{
	if (canScan) {
		canScan = false;

		FVector spawnLocation = GetActorLocation();
		FRotator spawnRotation = GetActorRotation();

		if (ScannerClass) {
			GetWorld()->SpawnActor<AScanner>(ScannerClass, spawnLocation, spawnRotation);
		}

		

		GetWorld()->GetTimerManager().SetTimer(scanTimerHandle, [&]()
			{
				canScan = true;
			}, scanCD, false);

	}
}

void ADoomCharacter::play2DSound(USoundCue* soundCue)
{
	if (soundCue) {
		UGameplayStatics::PlaySound2D(this, soundCue);
	}
}





// void ADoomCharacter::WeaponBobTimelineFinished(){

// }

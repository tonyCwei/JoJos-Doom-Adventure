// Fill out your copyright notice in the Description page of Project Settings.


#include "BFG.h"
#include "DoomCharacter.h"
#include "Projectile/BaseProjectile.h"
#include "UI/PlayerHUD.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

ABFG::ABFG()
{
	myAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	myAudioComponent->SetupAttachment(RootComponent);
}

void ABFG::BeginPlay()
{
	Super::BeginPlay();

	WeaponFlipBookComponent->SetLooping(false);

	chargeTimeNeeded = ChargingFlipbook->GetTotalDuration();
}

void ABFG::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearTimer(BFGFlipbookHandle);
	GetWorldTimerManager().ClearTimer(BFGTimerHandle);
	GetWorldTimerManager().ClearTimer(BFGFireHandle);
	GetWorldTimerManager().ClearTimer(BFGAudioHandle);
}

//Fire Weapon now means start Chargeing
void ABFG::FireWeapon() {
    if (playerCharacter->getCell() < 30) {
		playEmptyMagSound();
		return;
	};

	chargeStartTime = GetWorld()->GetTimeSeconds();

    if (!isCharging ) {

		isCharging = true;

		
		
        

		//Start Charging
		WeaponFlipBookComponent->SetFlipbook(ChargingFlipbook);
		WeaponFlipBookComponent->Play();

		chargeStartTime = GetWorld()->GetTimeSeconds();


		myAudioComponent->SetSound(chargingSound);
		myAudioComponent->Play();


		if (fullychargedSound && myAudioComponent->Sound) {
			GetWorld()->GetTimerManager().SetTimer(BFGAudioHandle, [&]()
				{
					myAudioComponent->SetSound(fullychargedSound);
					myAudioComponent->Play();
				}, myAudioComponent->Sound->GetDuration(), false);
	
		}
	   
        

		
        //No need for fire rate since hold to fire
        
	    //GetWorld()->GetTimerManager().SetTimer(BFGTimerHandle, [&]()
	    //{
	    //bFireOnce = true;
	    //}, fireRate, false);  
    } 





}

void ABFG::ShootProjectle() {
    //SpawnActor
	FVector spawnLocation = LineTraceComponent->GetComponentLocation();
	FRotator spawnRotation = LineTraceComponent->GetComponentRotation();
	//FTransform SpawnTransform = LineTraceComponent->GetComponentTransform();
	
	if (ProjectileClass) {
		ABaseProjectile* Projectile = GetWorld()->SpawnActor<ABaseProjectile>(ProjectileClass, spawnLocation, spawnRotation);
		//ABaseProjectile* Projectile = GetWorld()->SpawnActorDeferred<ABaseProjectile>(ProjectileClass, SpawnTransform);
		if (Projectile) {
			//UE_LOG(LogTemp, Display, TEXT("SpawnActorDeferred"));
			Projectile->projectileDamage = weaponDamage;
			Projectile->SetOwner(this);
			//Projectile->FinishSpawning(SpawnTransform);
		}
	} else {
		UE_LOG(LogTemp, Error, TEXT("Empty ProjectileClass"));
	}

    //Decrease 30 cell
    playerCharacter->setCell(playerCharacter->getCell() - 30);
    playerCharacter->getPlayerHUD()->UpdateCell();

	
	WeaponFlipBookComponent->SetFlipbook(ShootingFlipbook);
	
	GetWorldTimerManager().ClearTimer(BFGAudioHandle);
	myAudioComponent->Stop();
	playWeaponSound();

    
}

void ABFG::StopFire()
{
	if (!isCharging || hasFired) return;

	float chargedTime = GetWorld()->GetTimeSeconds() - chargeStartTime;

	if (chargedTime < chargeTimeNeeded) {
		WeaponFlipBookComponent->SetPlayRate(2);
		WeaponFlipBookComponent->Reverse();


		if (myAudioComponent && restSound) {
			GetWorldTimerManager().ClearTimer(BFGAudioHandle);
			myAudioComponent->SetSound(restSound);
			myAudioComponent->Play();
		}
		
		
		GetWorld()->GetTimerManager().SetTimer(BFGFlipbookHandle, [&]()
			{
				isCharging = false;
				WeaponFlipBookComponent->SetPlayRate(1);
				WeaponFlipBookComponent->SetFlipbook(IdleFlipbook);
			}, 0.5, false);
	}
	else {
		ShootProjectle();
		hasFired = true;

		GetWorld()->GetTimerManager().SetTimer(BFGFireHandle, [&]()
			{
				isCharging = false;
				hasFired = false;
				WeaponFlipBookComponent->SetFlipbook(IdleFlipbook);
			}, WeaponFlipBookComponent->GetFlipbookLength(), false);
	}

	
	 
	
}


void ABFG::cancelCharging()
{
	
}

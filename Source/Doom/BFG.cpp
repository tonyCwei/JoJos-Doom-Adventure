// Fill out your copyright notice in the Description page of Project Settings.


#include "BFG.h"
#include "DoomCharacter.h"
#include "Projectile/BaseProjectile.h"
#include "UI/PlayerHUD.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"

void ABFG::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearTimer(BFGFireHandle);
	GetWorldTimerManager().ClearTimer(BFGTimerHandle);
}

void ABFG::FireWeapon() {
    if (playerCharacter->getCell() < 30) return;

    if (bFireOnce) {
        bFireOnce = false;
        //Fire projectile after fire animation finishes
        PlayFireAnimation();

        
	    GetWorld()->GetTimerManager().SetTimer(BFGFireHandle, [&]()
	    {
	    ShootProjectle();
        WeaponFlipBookComponent->SetFlipbook(IdleFlipbook);
	    }, WeaponFlipBookComponent->GetFlipbookLength(), false);  
        

        //control fire rate
        
	    GetWorld()->GetTimerManager().SetTimer(BFGTimerHandle, [&]()
	    {
	    bFireOnce = true;
	    }, fireRate, false);  
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

    
}
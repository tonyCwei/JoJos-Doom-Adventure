// Fill out your copyright notice in the Description page of Project Settings.


#include "Fist.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "DoomCharacter.h"
#include "Enemies/BaseEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"

void AFist::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    GetWorldTimerManager().ClearTimer(MeleeTimerHandle);
}

void AFist::FireWeapon() {
   //Super::FireWeapon();
   Punch();
   resetFlipbook();
}

 void AFist::Punch() {
    FVector lineTraceLocation = LineTraceComponent->GetComponentLocation();
    FVector lineTraceForward = UKismetMathLibrary::GetForwardVector(LineTraceComponent->GetComponentRotation());
 	FVector lineTraceEnd = lineTraceForward * lineTraceDistance + lineTraceLocation;
 	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {UEngineTypes::ConvertToObjectType(ECC_WorldStatic), 
 														 UEngineTypes::ConvertToObjectType(ECC_WorldDynamic), 	
 														 UEngineTypes::ConvertToObjectType(ECC_Pawn)};
	
 	TArray<AActor*> ActorsToIgnore = {Cast<AActor>(this), UGameplayStatics::GetPlayerCharacter(this,0)};
 	FHitResult HitResult;													 
	
 	//Line Trace
 	bool hasHit = UKismetSystemLibrary::LineTraceSingleForObjects(this->GetWorld(), 
 													lineTraceLocation, 
 													lineTraceEnd, 
 													ObjectTypes, 
 													false, 
 													ActorsToIgnore,
 													EDrawDebugTrace::Type::ForDuration, 
 													HitResult, 
 													true);
	
 	//Apply DMG
 	if (hasHit) {
 		AActor* HitActor = HitResult.GetActor();
 		AActor* myOwner = GetOwner();
 		AController* MyOwnerInstigator = GetInstigatorController();
 		auto DamageTypeClass = UDamageType::StaticClass();
 		UGameplayStatics::ApplyDamage(HitActor, weaponDamage, MyOwnerInstigator, this, DamageTypeClass);

        if (HitActor->ActorHasTag("enemy")) {
            //Knock Enemy Back with Fist
            ABaseEnemy* enemyCharacter = Cast<ABaseEnemy>(HitActor);

            FVector launchDirection = enemyCharacter->GetActorLocation() - playerCharacter->GetActorLocation();
            launchDirection.Normalize();

            enemyCharacter->GetCharacterMovement()->StopMovementKeepPathing();
            FVector LaunchVelocity(launchDirection.X * 3000, launchDirection.Y * 3000, 0);
            enemyCharacter->LaunchCharacter(LaunchVelocity, true, false);

            ////Spawn Blood
            //if (bloodToSpawn) {
            //    FRotator spawnRotation = UKismetMathLibrary::FindLookAtRotation(HitResult.Location, playerCharacter->GetActorLocation());
            //    GetWorld()->SpawnActor<AActor>(bloodToSpawn, HitResult.Location, spawnRotation);
            //}
        }
 	}

     PlayFireAnimation();

   
 	GetWorld()->GetTimerManager().SetTimer(MeleeTimerHandle, [&]()
 	{
 	    WeaponFlipBookComponent->SetFlipbook(IdleFlipbook);	   
 	}, WeaponFlipBookComponent->GetFlipbookLength(), false);
   
 }
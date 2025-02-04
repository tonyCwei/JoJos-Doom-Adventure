// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Enemies/BaseEnemy.h"
#include "Math/RotationMatrix.h"
#include "Kismet/KismetMathLibrary.h"
#include "DoomCharacter.h"

void AShotgun::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    GetWorldTimerManager().ClearTimer(ShotgunTimerHandle);
}

void AShotgun::FireWeapon() {
    if (!hasEnoughAmmo()) {
        playEmptyMagSound();
        return;
    }
    

    
    if (bFireOnce) {
        bFireOnce = false;
        
        //fire pellets
        FVector lineTraceLocation = LineTraceComponent->GetComponentLocation();
        FRotator lineTraceRotation = LineTraceComponent->GetComponentRotation();
        FVector lineTraceForward = lineTraceRotation.Vector();
        

        // Shotgun spread parameters
        FVector Right = LineTraceComponent->GetRightVector();
        FVector Up = LineTraceComponent->GetUpVector();

        // Convert spread angle to radians and calculate spread factor
        float SpreadRad = FMath::DegreesToRadians(spreadAngle);
        float MaxSpreadRadius = FMath::Tan(SpreadRad);
		constexpr float GoldenAngle = 2.39996f;


        for (int32 i = 0; i < PelletCount; ++i)
        {
            // Calculate normalized index [0, 1]
            float NormalizedIndex = static_cast<float>(i) / PelletCount;

            // Spread radius increases with sqrt to distribute pellets uniformly
            float Radius = MaxSpreadRadius * FMath::Sqrt(NormalizedIndex);

            // Calculate angle using the golden angle spiral
            float Angle = GoldenAngle * i;

            // Convert polar coordinates (radius/angle) to 2D offset
            float OffsetX = Radius * FMath::Cos(Angle);
            float OffsetY = Radius * FMath::Sin(Angle);

            // Convert 2D offset to 3D direction using right/up vectors
            FVector PelletDirection =
                lineTraceForward +
                (Right * OffsetX) +
                (Up * OffsetY);

            PelletDirection.Normalize();

            // Calculate final direction and end point
            FVector PelletEnd = lineTraceLocation + (PelletDirection * lineTraceDistance);


			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_WorldStatic),
															UEngineTypes::ConvertToObjectType(ECC_WorldDynamic),
															UEngineTypes::ConvertToObjectType(ECC_Pawn),
															UEngineTypes::ConvertToObjectType(ECC_Destructible)
			};

			TArray<AActor*> ActorsToIgnore = { Cast<AActor>(this), UGameplayStatics::GetPlayerCharacter(this,0) };
			FHitResult HitResult;

			//Line Trace
			bool hasHit = UKismetSystemLibrary::LineTraceSingleForObjects(this->GetWorld(),
				lineTraceLocation,
				PelletEnd,
				ObjectTypes,
				false,
				ActorsToIgnore,
				EDrawDebugTrace::Type::None,
				HitResult,
				true);

			//Apply DMG
			if (hasHit) {
				AActor* HitActor = HitResult.GetActor();
				AActor* myOwner = GetOwner();
				AController* MyOwnerInstigator = GetInstigatorController();
				auto DamageTypeClass = UDamageType::StaticClass();
				UGameplayStatics::ApplyDamage(HitActor, weaponDamage, MyOwnerInstigator, this, DamageTypeClass);


				if (HitActor->ActorHasTag("Enemy")) {

					ABaseEnemy* hitEnemy = Cast<ABaseEnemy>(HitActor);

					if (hitEnemy && !hitEnemy->isInvin && bloodToSpawn) {
						FRotator spawnRotation = UKismetMathLibrary::FindLookAtRotation(HitResult.Location, playerCharacter->GetActorLocation());
						AActor* blood = GetWorld()->SpawnActor<AActor>(bloodToSpawn, HitResult.Location, spawnRotation);
						blood->AttachToActor(hitEnemy, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));

					}
					playWeaponHitSound();
				}
				else if (ammoType == Bullet || ammoType == Shell) {
					if (hitVFX) {

						FRotator spwanRotation = UKismetMathLibrary::MakeRotFromZ(HitResult.Normal);


						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), hitVFX, HitResult.Location, spwanRotation);
					}



				}


			}


        }



        //Decrease Ammo
        decreaseAmmo();

        //PlaySound
        playWeaponSound();

        //Animation
        PlayFireAnimation();


        resetFlipbook();

        //control fire rate
        
	    GetWorld()->GetTimerManager().SetTimer(ShotgunTimerHandle, [&]()
	    {
	    bFireOnce = true;
	    }, fireRate, false);  
    } 
}
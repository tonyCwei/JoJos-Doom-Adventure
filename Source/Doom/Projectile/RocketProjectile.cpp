// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketProjectile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PaperFlipbookComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Doom/DoomCharacter.h"

void ARocketProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	TArray<FHitResult> HitResults;
	FVector actorLocation = this->GetActorLocation();
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_WorldStatic),
														 UEngineTypes::ConvertToObjectType(ECC_WorldDynamic),
														 UEngineTypes::ConvertToObjectType(ECC_Pawn) };
	TArray<AActor*> ActorsToIgnore = { Cast<AActor>(this) };

	bool hasHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		this->GetWorld(),
		actorLocation,
		actorLocation,
		explosionRadius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::Type::ForDuration,
		HitResults,
		true
	);


	if (hasHit) {
		for (FHitResult hitResult : HitResults) {
			AActor* HitActor = hitResult.GetActor();
			AController* MyInstigator = GetInstigatorController();
			auto DamageTypeClass = UDamageType::StaticClass();

			
			if (!HitActor->ActorHasTag("Player")) {
				UGameplayStatics::ApplyDamage(HitActor, projectileDamage, MyInstigator, this, DamageTypeClass);
			}

			

		}

	}

	sphereCollisionDamage->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	projectileMovement->StopMovementImmediately();
	//Set flipbook and destroy, may need set scale
	projectileFlipbookComponent->SetWorldScale3D(destroyScale);

	//Set explosion flipbook location to make it look better
	FVector relativeLocation = projectileFlipbookComponent->GetRelativeLocation();
	FVector newRelativeLocation;

	float Distance = FVector::Dist(this->GetActorLocation(), UGameplayStatics::GetPlayerCharacter(this, 0)->GetActorLocation());

	if (Distance <= explosionRadius) {
		newRelativeLocation = FVector(relativeLocation.X, relativeLocation.Y, relativeLocation.Z + 50);
	}
	else {
		newRelativeLocation = FVector(relativeLocation.X - explosionRadius / 2, relativeLocation.Y, relativeLocation.Z + 50);
	}
	projectileFlipbookComponent->SetRelativeLocation(newRelativeLocation);
	
	projectileFlipbookComponent->SetFlipbook(destroyFlipbook);

	//Destroy after destroyFlipbook finishes playing


	GetWorld()->GetTimerManager().SetTimer(ProjectileTimerHandle, [&]()
		{
			Destroy();
		}, projectileFlipbookComponent->GetFlipbookLength() * 0.9, false);
	//multiplied 0.9 for delay time in order to prevent flipbook from looping back to the first frame
}

void ARocketProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) {
		TArray<FHitResult> HitResults;
		FVector actorLocation = this->GetActorLocation();
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_WorldStatic),
															 UEngineTypes::ConvertToObjectType(ECC_WorldDynamic),
															 UEngineTypes::ConvertToObjectType(ECC_Pawn) };
		TArray<AActor*> ActorsToIgnore = { Cast<AActor>(this) };

		bool hasHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
			this->GetWorld(),
			actorLocation,
			actorLocation,
			explosionRadius,
			ObjectTypes,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::Type::ForDuration,
			HitResults,
			true
		);


		if (hasHit) {
			for (FHitResult hitResult : HitResults) {
				AActor* HitActor = hitResult.GetActor();
				AController* MyInstigator = GetInstigatorController();
				auto DamageTypeClass = UDamageType::StaticClass();


				if (!HitActor->ActorHasTag("Player")) {
					UGameplayStatics::ApplyDamage(HitActor, projectileDamage, MyInstigator, this, DamageTypeClass);
				}



			}

		}

		sphereCollisionDamage->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		projectileMovement->StopMovementImmediately();
		//Set flipbook and destroy, may need set scale
		projectileFlipbookComponent->SetWorldScale3D(destroyScale);

		//Set explosion flipbook location to make it look better
		FVector relativeLocation = projectileFlipbookComponent->GetRelativeLocation();
		FVector newRelativeLocation;

		float Distance = FVector::Dist(this->GetActorLocation(), UGameplayStatics::GetPlayerCharacter(this, 0)->GetActorLocation());

		if (Distance <= explosionRadius) {
			newRelativeLocation = FVector(relativeLocation.X, relativeLocation.Y, relativeLocation.Z + 50);
		}
		else {
			newRelativeLocation = FVector(relativeLocation.X - explosionRadius / 2, relativeLocation.Y, relativeLocation.Z + 50);
		}
		projectileFlipbookComponent->SetRelativeLocation(newRelativeLocation);

		projectileFlipbookComponent->SetFlipbook(destroyFlipbook);

		//Destroy after destroyFlipbook finishes playing


		GetWorld()->GetTimerManager().SetTimer(ProjectileTimerHandle, [&]()
			{
				Destroy();
			}, projectileFlipbookComponent->GetFlipbookLength() * 0.9, false);
		//multiplied 0.9 for delay time in order to prevent flipbook from looping back to the first frame

	}
}

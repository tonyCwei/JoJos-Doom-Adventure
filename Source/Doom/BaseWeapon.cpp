// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "DoomCharacter.h"
#include "UI/PlayerHUD.h"
#include "Projectile/BaseProjectile.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "Sound/SoundCue.h"
#include "Enemies/BaseEnemy.h"
#include "Particles/ParticleSystem.h"
#include "Math/RotationMatrix.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Prop/WoodCrate.h"
#include "Camera/CameraShakeBase.h"
#include "Camera/PlayerCameraManager.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent) {
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	}

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(RootComponent);

	WeaponFlipBookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("WeaponFlipBook"));
	WeaponFlipBookComponent->SetupAttachment(RootComponent);
	WeaponFlipBookComponent->SetRelativeRotation(FRotator(0.f,90.f,0.f));
	WeaponFlipBookComponent->SetWorldScale3D(FVector(0.07,0.07,0.07));

	LineTraceComponent = CreateDefaultSubobject<USceneComponent>(TEXT("LineTrace"));
	LineTraceComponent->SetupAttachment(RootComponent);
	LineTraceComponent->SetRelativeLocation(FVector(2.f,0.f,0.f));

	
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	IdleFlipbook = WeaponFlipBookComponent->GetFlipbook();
	playerCharacter = Cast<ADoomCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	
}

void ABaseWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearTimer(resetFlipbookTimerHandle);
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void ABaseWeapon::FireWeapon(){
	//UE_LOG(LogTemp, Warning, TEXT("BaseWeapon Fire"));

	if (!hasEnoughAmmo()) {
		playEmptyMagSound();
		return;
	}
	//Parameters for line trace for objects
	FVector lineTraceLocation = LineTraceComponent->GetComponentLocation();
    FVector lineTraceForward = UKismetMathLibrary::GetForwardVector(LineTraceComponent->GetComponentRotation());
	FVector lineTraceEnd = lineTraceForward * lineTraceDistance + lineTraceLocation;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {	UEngineTypes::ConvertToObjectType(ECC_WorldStatic), 
															UEngineTypes::ConvertToObjectType(ECC_WorldDynamic), 	
															UEngineTypes::ConvertToObjectType(ECC_Pawn),
															UEngineTypes::ConvertToObjectType(ECC_Destructible) 
														};
	
	TArray<AActor*> ActorsToIgnore = {Cast<AActor>(this), UGameplayStatics::GetPlayerCharacter(this,0)};
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
			
			//For wooden box
			if (HitActor->ActorHasTag("WoodCrate")) {
				AWoodCrate* hitCrate = Cast<AWoodCrate>(HitActor);
				hitCrate->Destruct(HitResult.Location);
			}
		}
	}

	//Decrease Ammo
	decreaseAmmo();

	//PlayFX
	playWeaponSound();
	playCameraShake();
	
	//Animation
	PlayFireAnimation();
}

//For Weapons shoot projectle instead of line trace
void ABaseWeapon::ShootProjectle() {
	if (!hasEnoughAmmo()) {
		playEmptyMagSound();
		return;
	}
	
	//UE_LOG(LogTemp, Display, TEXT("Shoot projectle"));
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
			Projectile->SetOwner(UGameplayStatics::GetPlayerCharacter(this, 0));
			//Projectile->FinishSpawning(SpawnTransform);
		}
	} else {
		UE_LOG(LogTemp, Error, TEXT("Empty ProjectileClass"));
	}
	

	
	//Decrease Ammo
	decreaseAmmo();

	//Play Sound
	playWeaponSound();
	playCameraShake();

	//Animation
	PlayFireAnimation();
}

void ABaseWeapon::PlayFireAnimation() {
	WeaponFlipBookComponent->SetFlipbook(ShootingFlipbook);
}

void ABaseWeapon::resetFlipbook() {
	
	GetWorld()->GetTimerManager().SetTimer(resetFlipbookTimerHandle, [&]()
	{
	   WeaponFlipBookComponent->SetFlipbook(IdleFlipbook);	   
	}, WeaponFlipBookComponent->GetFlipbookLength()*0.9, false);
	//multiplied 0.9 for delay time in order to prevent flipbook from looping back to the first frame
}


//for hold fire weapon only
void ABaseWeapon::StopFire() {}

// //for fist
// void ABaseWeapon::Punch() {
	
// }

bool ABaseWeapon::hasEnoughAmmo() {
	switch (ammoType)
	{
	case Bullet:
		return playerCharacter->getBullet() > 0;	
	
	case Shell:
		return playerCharacter->getShell() > 0;
		
	case Rocket:
		return playerCharacter->getRocket() > 0;
	
	case Cell:
		return playerCharacter->getCell() > 0;
	
	case MeleeWeapon:
		return true;
	
	default:
		return false;
	}
}

void ABaseWeapon::decreaseAmmo() {
	switch (ammoType)
	{
	case Bullet:
		 playerCharacter->decreaseBullet();	
		 playerCharacter->getPlayerHUD()->UpdateBullet();
		 break;
	
	case Shell:
		 playerCharacter->decreaseShell();
		 playerCharacter->getPlayerHUD()->UpdateShell();
		 break;
		
	case Rocket:
		 playerCharacter->decreaseRocket();
		 playerCharacter->getPlayerHUD()->UpdateRocket();
		 break;
	
	case Cell:
		 playerCharacter->decreaseCell();
		 playerCharacter->getPlayerHUD()->UpdateCell();
		 break;
	
	default:
		return;
	}
}

void ABaseWeapon::playCameraShake()
{
	// Ensure the camera shake class is valid
	if (!CameraShakeClass) return;

	// Get the owning player's controller
	/*APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;*/

	APlayerController* PC = Cast<APlayerController>(playerCharacter->GetController());
	if (!PC) return;

	// Play the camera shake via the player's camera manager
	/*if (PC->PlayerCameraManager) {
		PC->PlayerCameraManager->StartCameraShake(
			CameraShakeClass,
			1.0f 
		);
	}*/

	if (PC) PC->ClientStartCameraShake(CameraShakeClass);

}

void ABaseWeapon::playWeaponSound()
{
	if (weaponSound) {
		UGameplayStatics::PlaySoundAtLocation(this, weaponSound, this->GetActorLocation());
	}
	
}

void ABaseWeapon::playWeaponSwapSound()
{
	if (weaponSwapSound) {
		UGameplayStatics::PlaySoundAtLocation(this, weaponSwapSound, this->GetActorLocation());
	}
}

void ABaseWeapon::playWeaponHitSound()
{

	if (weaponHitSound) {
		UGameplayStatics::PlaySoundAtLocation(this, weaponHitSound, this->GetActorLocation());
	}
}

void ABaseWeapon::playEmptyMagSound()
{
	if (emptyMagSound) {
		UGameplayStatics::PlaySoundAtLocation(this, emptyMagSound, this->GetActorLocation());
	}
}

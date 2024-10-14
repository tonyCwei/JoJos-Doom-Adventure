// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Doom/Ability/BulletTimeAura.h"
#include "GameFramework/ProjectileMovementComponent.h"

AEnemyProjectile::AEnemyProjectile() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxCollisionDodge = CreateDefaultSubobject<UBoxComponent>(TEXT("boxCollisionDodge"));
	boxCollisionDodge ->SetupAttachment(RootComponent);
}







void AEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	//move to enemyprojectile
	sphereCollisionDamage->OnComponentEndOverlap.AddDynamic(this, &AEnemyProjectile::EndOverlap);
	boxCollisionDodge->OnComponentBeginOverlap.AddDynamic(this, &AEnemyProjectile::BeginOverlapBoxDodge);
}

void AEnemyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	UE_LOG(LogTemp, Display, TEXT("Enemy On hit: %s"), *OtherActor->GetName());


	//Apply Damage
	auto MyInstigator = GetInstigatorController();
	auto DamageTypeClass = UDamageType::StaticClass();

	if (OtherActor && OtherActor != this && !OtherActor->ActorHasTag("Projectile")) {
		if (OtherActor->ActorHasTag("enemy")) {
			UGameplayStatics::ApplyDamage(OtherActor, 1, MyInstigator, this, DamageTypeClass);
		}
		else {
			UGameplayStatics::ApplyDamage(OtherActor, projectileDamage, MyInstigator, this, DamageTypeClass);
		}

	}


	sphereCollisionDamage->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	boxCollisionDodge->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//Set flipbook and destroy, may need set scale
	projectileFlipbookComponent->SetWorldScale3D(destroyScale);
	projectileFlipbookComponent->SetFlipbook(destroyFlipbook);

	//Destroy after destroyFlipbook finishes playing

	if (isAdded) {
		gameStateRef->removeAttack(curAttackInfo);
	}

	GetWorld()->GetTimerManager().SetTimer(ProjectileTimerHandle, [&]()
		{
			Destroy();
		}, projectileFlipbookComponent->GetFlipbookLength() * 0.9, false);
	//multiplied 0.9 for delay time in order to prevent flipbook from looping back to the first frame
}

void AEnemyProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("BulletTimeAura")) {
		//UE_LOG(LogTemp, Display, TEXT("SlowTime"));
		ABulletTimeAura* myBulletTimeAura = Cast<ABulletTimeAura>(OtherActor);
		if (myBulletTimeAura) {
			this->CustomTimeDilation = myBulletTimeAura->getSlowDownRate();
		}

	}
	else {


		UE_LOG(LogTemp, Display, TEXT("Enemy On BeginOverlap"));


		//Apply Damage
		auto MyInstigator = GetInstigatorController();
		auto DamageTypeClass = UDamageType::StaticClass();

		if (OtherActor && OtherActor != this && !OtherActor->ActorHasTag("Projectile")) {
			if (OtherActor->ActorHasTag("Player")) {
				UGameplayStatics::ApplyDamage(OtherActor, projectileDamage, MyInstigator, this, DamageTypeClass);
			}
			else {
				UGameplayStatics::ApplyDamage(OtherActor, 1, MyInstigator, this, DamageTypeClass);
			}
			
		}

		sphereCollisionDamage->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		boxCollisionDodge->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		projectileMovement->StopMovementImmediately();

		//Set flipbook and destroy, may need set scale
		projectileFlipbookComponent->SetWorldScale3D(destroyScale);
		projectileFlipbookComponent->SetFlipbook(destroyFlipbook);

		//Destroy after destroyFlipbook finishes playing

		if (isAdded) {
			gameStateRef->removeAttack(curAttackInfo);
		}

		GetWorld()->GetTimerManager().SetTimer(ProjectileTimerHandle, [&]()
			{
				Destroy();
			}, projectileFlipbookComponent->GetFlipbookLength() * 0.9, false);
		//multiplied 0.9 for delay time in order to prevent flipbook from looping back to the first frame
	
	
	}


}

void AEnemyProjectile::EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UE_LOG(LogTemp, Display, TEXT("End Overlap"));
	if (OtherActor->ActorHasTag("BulletTimeAura")) {
		//UE_LOG(LogTemp, Display, TEXT("SlowTime"));
		this->CustomTimeDilation = 1;
	}
}

void AEnemyProjectile::BeginOverlapBoxDodge(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ( OtherActor->ActorHasTag("Player")) {

		UE_LOG(LogTemp, Display, TEXT("BeginOverlapBoxDodge"));


		curAttackInfo.StartTime = GetWorld()->GetTimeSeconds();
		curAttackInfo.Duration = attackDuration;
		curAttackInfo.Attacker = this;

		gameStateRef->addAttack(curAttackInfo);
		isAdded = true;

	}
}

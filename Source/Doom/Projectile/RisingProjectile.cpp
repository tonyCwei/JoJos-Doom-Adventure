// Fill out your copyright notice in the Description page of Project Settings.


#include "RisingProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Doom/Ability/BulletTimeAura.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "Doom/DoomCharacter.h"
#include "Components/TimelineComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"



ARisingProjectile::ARisingProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	riseTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorOpenTimeline"));

	sunAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("sunAudio"));
	sunAudioComponent->SetupAttachment(RootComponent);
}

void ARisingProjectile::BeginPlay()
{
	Super::BeginPlay();

	playerCharacter = Cast<ADoomCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	OnTakeAnyDamage.AddDynamic(this, &ARisingProjectile::DamageTaken);

	riseStartPoint = GetActorLocation();
	riseEndPoint = riseStartPoint + FVector(0,0, riseHeight);

	//Timeline Binding
	if (FloatCurve) {
		// Bind the update function
		FOnTimelineFloat UpdateFunction;
		UpdateFunction.BindUFunction(this, FName("riseTimelineUpdate"));

		// Bind the finished function
		FOnTimelineEvent FinishedFunction;
		FinishedFunction.BindUFunction(this, FName("riseTimelineFinished"));

		// Add functions to the timeline
		riseTimeline->AddInterpFloat(FloatCurve, UpdateFunction);
		riseTimeline->SetTimelineFinishedFunc(FinishedFunction);

		// Set the timeline to loop or play once
		riseTimeline->SetLooping(false);

	}

	if (riseTimeline) {
		riseTimeline->Play();
	
	}
}

void ARisingProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	rotateToPlayer(DeltaTime);
}

void ARisingProjectile::DestroySelf()
{
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

	UE_LOG(LogTemp, Display, TEXT("DestroySelf"));
}

void ARisingProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//Apply Damage
	auto MyInstigator = GetInstigatorController();
	auto DamageTypeClass = UDamageType::StaticClass();
	
	UE_LOG(LogTemp, Display, TEXT("RisingSun OnHit"));

	if (OtherActor && OtherActor != this && !OtherActor->ActorHasTag("Projectile") && !OtherActor->ActorHasTag("Enemy")) {
		if (OtherActor->ActorHasTag("Player")) {
			UGameplayStatics::ApplyDamage(OtherActor, projectileDamage, MyInstigator, this, DamageTypeClass);
		}
		DestroySelf();
	}

	
}

void ARisingProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	UE_LOG(LogTemp, Display, TEXT("RisingSun On BeginOverlap"));

	if (OtherActor->ActorHasTag("BulletTimeAura")) {
		//UE_LOG(LogTemp, Display, TEXT("SlowTime"));
		ABulletTimeAura* myBulletTimeAura = Cast<ABulletTimeAura>(OtherActor);
		if (myBulletTimeAura) {
			this->CustomTimeDilation = myBulletTimeAura->getSlowDownRate();
		}

	}
	else  {


		


		//Apply Damage
		if (OtherActor && OtherActor != this && !OtherActor->ActorHasTag("Projectile") && !OtherActor->ActorHasTag("Enemy")) {
			auto MyInstigator = GetInstigatorController();
			auto DamageTypeClass = UDamageType::StaticClass();
			if (OtherActor->ActorHasTag("Player")) {
				UGameplayStatics::ApplyDamage(OtherActor, projectileDamage, MyInstigator, this, DamageTypeClass);
			}
			DestroySelf();
		}
		

		
	}

	
}

void ARisingProjectile::riseTimelineUpdate(float Value)
{
	FVector newLocation = FMath::Lerp(riseStartPoint, riseEndPoint, Value);
	this->SetActorLocation(newLocation);
}

void ARisingProjectile::riseTimelineFinished()
{

	
	//FTimerHandle activateTimerHandle;
	/*GetWorld()->GetTimerManager().SetTimer(activateTimerHandle, [&]()
		{
			projectileMovement->InitialSpeed = shootSpeed;
			projectileMovement->MaxSpeed = shootSpeed;

			projectileMovement->Velocity = GetActorForwardVector() * projectileMovement->InitialSpeed;
			projectileMovement->Activate();

			if (sunShootSound) {
				UGameplayStatics::PlaySoundAtLocation(this, sunShootSound, this->GetActorLocation());
			}

			sunAudioComponent->Deactivate();
		}, 3, false);*/


	projectileMovement->InitialSpeed = shootSpeed;
	projectileMovement->MaxSpeed = shootSpeed;

	projectileMovement->Velocity = GetActorForwardVector() * projectileMovement->InitialSpeed;
	projectileMovement->Activate();

	if (sunShootSound) {
		UGameplayStatics::PlaySoundAtLocation(this, sunShootSound, this->GetActorLocation());
	}

	sunAudioComponent->Deactivate();
	
	sphereCollisionDamage->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	sphereCollisionDamage->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	
}

void ARisingProjectile::rotateToPlayer(float DeltaTime)
{
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), playerCharacter->GetActorLocation());

	FRotator NewRotation = FMath::RInterpConstantTo(this->GetActorRotation(), TargetRotation, DeltaTime, 50);

	//this->SetActorRotation(NewRotation);

	this->SetActorRotation(NewRotation);
}

void ARisingProjectile::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* DamageInstigator, AActor* DamageCauser)
{
	curHealth -= Damage;

	if (sunHitSound) {
		UGameplayStatics::PlaySoundAtLocation(this, sunHitSound, this->GetActorLocation());
	}
	
	if (curHealth <= 0) {
		curHealth = 0;
		DestroySelf();
	}
	else {
		//projectileFlipbookComponent->SetSpriteColor(FLinearColor(1.0f, 1.0f, 0.0f, 1.0f));

		projectileFlipbookComponent->SetFlipbook(destroyFlipbook);

		FTimerHandle damagedTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(damagedTimerHandle, [&]()
			{
				//projectileFlipbookComponent->SetSpriteColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
				projectileFlipbookComponent->SetFlipbook(idleFlipbook);
			}, 0.1, false);
	
	}



}

// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseProjectile.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"




// Sets default values
ABaseProjectile::ABaseProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Setting up components
	// if (!RootComponent) {
	// 	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	// }
	
	sphereCollisionDamage = CreateDefaultSubobject<USphereComponent>(TEXT("sphereCollisionDamage"));
	RootComponent = sphereCollisionDamage;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(RootComponent);

	projectileFlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("ProjectileFlipBook"));
	projectileFlipbookComponent->SetupAttachment(RootComponent);

	
	projectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	//projectileMovement->SetupAttachment(RootComponent);
	//projectileMovement->InitialSpeed = projectileSpeed;
	//projectileMovement->MaxSpeed = projectileSpeed;
	projectileMovement->ProjectileGravityScale = 0.0f;

	//Setting life span of the projectile
	//InitialLifeSpan = 3.0f;



}

void ABaseProjectile::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
	projectileMovement->InitialSpeed = projectileSpeed;
	projectileMovement->MaxSpeed = projectileSpeed;
}



// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	//Game State Ref
	gameStateRef = GetWorld()->GetGameState<ADoomGameStateBase>();

	sphereCollisionDamage->OnComponentHit.AddDynamic(this, &ABaseProjectile::OnHit);
	sphereCollisionDamage->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::BeginOverlap);
	
	
	
}

void ABaseProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearTimer(ProjectileTimerHandle);
}

// Called every frame
void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ABaseProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	
		//AActor* MyOwner = GetOwner();
		//if (MyOwner == nullptr) {
		//	Destroy();
		//	return;
		//}


		UE_LOG(LogTemp, Display, TEXT("On hit: %s"), *OtherActor->GetName());


		//Apply Damage
		auto MyInstigator = GetInstigatorController();
		auto DamageTypeClass = UDamageType::StaticClass();

		if (OtherActor && OtherActor != this && !OtherActor->ActorHasTag("Projectile")) {
				UGameplayStatics::ApplyDamage(OtherActor, projectileDamage, MyInstigator, this, DamageTypeClass);
		}


		sphereCollisionDamage->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		projectileMovement->StopMovementImmediately();

		//Set flipbook and destroy, may need set scale
		projectileFlipbookComponent->SetWorldScale3D(destroyScale);
		projectileFlipbookComponent->SetFlipbook(destroyFlipbook);


		//Destroy after destroyFlipbook finishes playing

	
		GetWorld()->GetTimerManager().SetTimer(ProjectileTimerHandle, [&]()
			{
				Destroy();
			}, projectileFlipbookComponent->GetFlipbookLength() * 0.9, false);
		//multiplied 0.9 for delay time in order to prevent flipbook from looping back to the first frame

		
		
}

void ABaseProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Display, TEXT("On BeginOverlap: %s"), *OtherActor->GetName());

	if (!OtherActor->ActorHasTag("Player")) {
		//Apply Damage
		auto MyInstigator = GetInstigatorController();
		auto DamageTypeClass = UDamageType::StaticClass();

		if (OtherActor && OtherActor != this && !OtherActor->ActorHasTag("Projectile")) {
			UGameplayStatics::ApplyDamage(OtherActor, projectileDamage, MyInstigator, this, DamageTypeClass);
		}

		sphereCollisionDamage->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		projectileMovement->StopMovementImmediately();

		//Set flipbook and destroy, may need set scale
		projectileFlipbookComponent->SetWorldScale3D(destroyScale);
		projectileFlipbookComponent->SetFlipbook(destroyFlipbook);

		//Destroy after destroyFlipbook finishes playing

		GetWorld()->GetTimerManager().SetTimer(ProjectileTimerHandle, [&]()
			{
				Destroy();
			}, projectileFlipbookComponent->GetFlipbookLength() * 0.9, false);
		//multiplied 0.9 for delay time in order to prevent flipbook from looping back to the first frame

	}
		
	
	}



	









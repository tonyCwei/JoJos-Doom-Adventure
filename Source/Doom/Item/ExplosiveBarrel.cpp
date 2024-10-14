// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveBarrel.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Doom/DoomCharacter.h"

// Sets default values
AExplosiveBarrel::AExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	boxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	RootComponent = boxCollision;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(RootComponent);

	myFlipBookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("MyPaperFilpbook"));
	myFlipBookComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	OnTakeAnyDamage.AddDynamic(this, &AExplosiveBarrel::DamageTaken);


	playerCharacterRef = Cast<ADoomCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

}

// Called every frame
void AExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	rotateToPlayer(DeltaTime);
}

void AExplosiveBarrel::rotateToPlayer(float DeltaTime)
{
	if (playerCharacterRef) {
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), playerCharacterRef->GetActorLocation());
		this->SetActorRotation(FRotator(0, TargetRotation.Yaw, 0));

	}
	
}

void AExplosiveBarrel::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* DamageInstigator, AActor* DamageCauser)
{
	myFlipBookComponent->SetLooping(false);
	FVector ExplosionScale(10, 10, 10);
	myFlipBookComponent->SetWorldScale3D(ExplosionScale);
	
	FVector relativeLocation = myFlipBookComponent->GetRelativeLocation();
	FVector newRelativeLocation;
	
	float Distance = FVector::Dist(this->GetActorLocation(), playerCharacterRef->GetActorLocation());
	
	if (Distance <= explosionRadius) {
		newRelativeLocation = FVector(relativeLocation.X + Distance - 250, relativeLocation.Y, relativeLocation.Z + 50);
	}
	else {
		newRelativeLocation = FVector(relativeLocation.X + explosionRadius / 2, relativeLocation.Y, relativeLocation.Z + 50);
	}
	

	myFlipBookComponent->SetRelativeLocation(newRelativeLocation);




	myFlipBookComponent->SetFlipbook(explosionFlipbook);


	TArray<FHitResult> HitResults;
	FVector actorLocation = this->GetActorLocation();
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_Pawn) };
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
			
			float actualDamage;
			if (HitActor->ActorHasTag("Player")) {
				actualDamage = playerDamage;
			}
			else {
				actualDamage = enemyDamage;
			}
			UGameplayStatics::ApplyDamage(HitActor, actualDamage, MyInstigator, this, DamageTypeClass);

		}

	}

	FTimerHandle destroyTimerHandle;
	GetWorldTimerManager().SetTimer(destroyTimerHandle, [&]() {
		Destroy();
		}, myFlipBookComponent->GetFlipbookLength(), false);

}


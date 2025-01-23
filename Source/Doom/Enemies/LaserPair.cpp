// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserPair.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"


// Sets default values
ALaserPair::ALaserPair()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	laserStartPoint = CreateDefaultSubobject<USceneComponent>(TEXT("laserStartPoint"));
	RootComponent = laserStartPoint;
	
	
	
	
	laserStartMesh =  CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserStartMesh"));
	laserStartMesh->SetupAttachment(RootComponent);

	laserEndMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserEndMesh"));
	laserEndMesh->SetupAttachment(RootComponent);

	laserEndPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LaserEndPoint"));
	laserEndPoint->SetupAttachment(laserEndMesh);

	laser1VFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Laser1VFX"));
	laser1VFX->SetupAttachment(RootComponent);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("LaserAudio"));
	AudioComponent->SetupAttachment(RootComponent);

	

}

// Called when the game starts or when spawned
void ALaserPair::BeginPlay()
{
	Super::BeginPlay();
	
	

	laserAttackStartLocation = laserStartPoint->GetComponentLocation();

	
}

// Called every frame
void ALaserPair::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	laserAttack(DeltaTime);
}

void ALaserPair::laserHasHurt()
{
	GetWorldTimerManager().ClearTimer(laserHurtTimerHandle);
	canLaserHurt = false;

	GetWorld()->GetTimerManager().SetTimer(laserHurtTimerHandle, [&]()
		{
			canLaserHurt = true;
		}, laserHurtInterval, false);
}

void ALaserPair::laserAttack(float DeltaTime)
{
	//Move Laser
	FVector curLocation = this->GetActorLocation();

	FVector newLocation = FMath::VInterpConstantTo(curLocation, laserAttackEndLocation, DeltaTime, laserMoveSpeed);
	this->SetActorLocation(newLocation);
	
	if (FVector::Dist(newLocation, laserAttackEndLocation) == 0) {
		FVector temp = laserAttackStartLocation;
		laserAttackStartLocation = laserAttackEndLocation;
		laserAttackEndLocation = temp;
	}

	if (laser1VFX) {
		laser1VFX->SetVectorParameter("Beam End", laserEndPoint->GetComponentLocation());
	}
	
	//Shoot trace
	FVector HalfSize(25.0f, 25.0f, 0.0f);
	/*TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_WorldStatic),
		UEngineTypes::ConvertToObjectType(ECC_WorldDynamic),
		UEngineTypes::ConvertToObjectType(ECC_Pawn),
		UEngineTypes::ConvertToObjectType(ECC_Destructible)
	};*/
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {
		UEngineTypes::ConvertToObjectType(ECC_Pawn)	
	};

	TArray<AActor*> ActorsToIgnore = {};

	FHitResult HitResult;

	bool hasHit = UKismetSystemLibrary::BoxTraceSingleForObjects(
		this->GetWorld(),
		laserStartPoint->GetComponentLocation(),
		laserEndPoint->GetComponentLocation(),
		HalfSize,
		FRotator::ZeroRotator,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::Type::None,
		HitResult,
		true
	);

	if (hasHit) {
		
		UE_LOG(LogTemp, Display, TEXT("hasHit"));
		AActor* HitActor = HitResult.GetActor();

		if (HitActor->ActorHasTag("Player") && canLaserHurt) {
			UGameplayStatics::ApplyDamage(HitActor, laserDamage, GetInstigatorController(), this, UDamageType::StaticClass());
			laserHasHurt();
		}

	}

}


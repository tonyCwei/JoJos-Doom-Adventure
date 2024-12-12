// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserTower.h"
#include "Curves/CurveFloat.h"
#include "Components/TimelineComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALaserTower::ALaserTower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	tower = CreateDefaultSubobject<USceneComponent>(TEXT("Tower"));
	RootComponent = tower;

	laser1 = CreateDefaultSubobject<USceneComponent>(TEXT("Laser1"));
	laser1->SetupAttachment(RootComponent);

	laser1VFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Laser1VFX"));
	laser1VFX->SetupAttachment(laser1);


	laser2 = CreateDefaultSubobject<USceneComponent>(TEXT("Laser2"));
	laser2->SetupAttachment(RootComponent);

	laser2VFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Laser2VFX"));
	laser2VFX->SetupAttachment(laser2);

	towerActivateTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TowerActivateTimeline"));

}

// Called when the game starts or when spawned
void ALaserTower::BeginPlay()
{
	Super::BeginPlay();
	
	if (towerRiseFloatCurve) {
		// Bind the update function
		FOnTimelineFloat UpdateFunction;
		UpdateFunction.BindUFunction(this, FName("towerActivateTimelineUpdate"));

		// Bind the finished function
		FOnTimelineEvent FinishedFunction;
		FinishedFunction.BindUFunction(this, FName("towerActivateTimelineFinished"));

		// Add functions to the timeline
		towerActivateTimeline->AddInterpFloat(towerRiseFloatCurve, UpdateFunction);
		towerActivateTimeline->SetTimelineFinishedFunc(FinishedFunction);

		// Set the timeline to loop or play once
		towerActivateTimeline->SetLooping(false);

	}

	deactivatedLocation = GetActorLocation();
	activatedLocation = deactivatedLocation + FVector(0, 0, riseDistance);
}

// Called every frame
void ALaserTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isLaserActive) {
		laserTick();
	}

	if (shouldTowerRotate) {
		rotateTower(DeltaTime);
	}
}

void ALaserTower::towerActivateTimelineUpdate(float Value)
{
	FVector newLocation = FMath::Lerp(deactivatedLocation, activatedLocation, Value);
	SetActorLocation(newLocation);
}

void ALaserTower::towerActivateTimelineFinished()
{
	if (isTowerActivation) {
		activateLaser();
		isTowerActivation = false;
	}
}



void ALaserTower::activateTower()
{
	isTowerActivation = true;
	towerActivateTimeline->Play();
}

void ALaserTower::deactivateTower()
{
	deactivateLaser();
	towerActivateTimeline->Reverse();
}

void ALaserTower::activateLaser()
{
	isLaserActive = true;
	shouldTowerRotate = true;

	laser1VFX->SetVisibility(true);
	laser2VFX->SetVisibility(true);
}

void ALaserTower::deactivateLaser()
{
	isLaserActive = false;
	shouldTowerRotate = false;

	laser1VFX->SetVisibility(false);
	laser2VFX->SetVisibility(false);

}


void ALaserTower::laserHasHurt()
{
	GetWorldTimerManager().ClearTimer(laserHurtTimerHandle);
	canLaserHurt = false;

	GetWorld()->GetTimerManager().SetTimer(laserHurtTimerHandle, [&]()
		{
			canLaserHurt = true;
		}, laserHurtInterval, false);

}

void ALaserTower::laserTick()
{
	FVector HalfSize(25.0f, 25.0f, 0.0f);
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_WorldStatic),
		UEngineTypes::ConvertToObjectType(ECC_WorldDynamic),
		UEngineTypes::ConvertToObjectType(ECC_Pawn),
		UEngineTypes::ConvertToObjectType(ECC_Destructible)
	};

	TArray<AActor*> ActorsToIgnore = {};
	
	//For Laser 1
	FVector laser1Start = laser1->GetComponentLocation();
	FVector laser1End = laser1->GetForwardVector() * laserLength + laser1Start;


	FHitResult HitResult1;

	bool hasHit1 = UKismetSystemLibrary::BoxTraceSingleForObjects(
		this->GetWorld(),
		laser1Start,
		laser1End,
		HalfSize,
		FRotator::ZeroRotator,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::Type::None,
		HitResult1,
		true
		);

	if (hasHit1) {
		laser1VFX->SetVectorParameter("Beam End", HitResult1.Location);

		AActor* HitActor = HitResult1.GetActor();
	
		if (HitActor->ActorHasTag("Player") && canLaserHurt) {
			UGameplayStatics::ApplyDamage(HitActor, laserDamage, GetInstigatorController(), this, UDamageType::StaticClass());
			laserHasHurt();
		}

	}


	//For Laser 2
	FVector laser2Start = laser2->GetComponentLocation();
	FVector laser2End = laser2->GetForwardVector() * laserLength + laser2Start;


	FHitResult HitResult2;

	bool hasHit2 = UKismetSystemLibrary::BoxTraceSingleForObjects(
		this->GetWorld(),
		laser2Start,
		laser2End,
		HalfSize,
		FRotator::ZeroRotator,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::Type::None,
		HitResult2,
		true
	);

	if (hasHit2) {
		laser2VFX->SetVectorParameter("Beam End", HitResult2.Location);

		AActor* HitActor2 = HitResult2.GetActor();

		if (HitActor2->ActorHasTag("Player") && canLaserHurt) {
			UGameplayStatics::ApplyDamage(HitActor2, laserDamage, GetInstigatorController(), this, UDamageType::StaticClass());
			laserHasHurt();
		}

	}


}


void ALaserTower::rotateTower(float DeltaTime)
{
	FRotator curRotation = GetActorRotation();

	FRotator targetRotation = curRotation + FRotator(0, towerRotationSpeed * DeltaTime, 0);

	this->SetActorRotation(targetRotation);

}



// Fill out your copyright notice in the Description page of Project Settings.


#include "Harubus.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "Doom/DoomCharacter.h"
#include "Kismet/GameplayStatics.h"
#include <Kismet/KismetMathLibrary.h>
#include "Doom/Projectile/BaseProjectile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveFloat.h"
#include "Components/TimelineComponent.h"
#include "Doom/Enemies/SelfDestructEnemy.h"
#include "LaserTower.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Doom/UI/BossHUD.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"

#include "Doom/Utility/BGMManager.h"
#include "Blueprint/UserWidget.h"


AHarubus::AHarubus()
{

	AnimationFlipBookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("AnimationFlipBookComponent"));
	AnimationFlipBookComponent->SetupAttachment(RootComponent);

	PromptChildActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("PromptChildActorComponent"));
	PromptChildActorComponent->SetupAttachment(RootComponent);
	PromptChildActorComponent->SetVisibility(false);

	AnimationFlipBookComponent->SetLooping(false);
	
	ProjectileSpawn2 = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawn2"));
	ProjectileSpawn2->SetupAttachment(RootComponent);

	onetwentyTimeline = CreateDefaultSubobject<UTimelineComponent> (TEXT("OnetwentyTimeline"));

	dropAttackTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DropAttackTimeline"));

	//Summon Attack
	SummonSpawn0 = CreateDefaultSubobject<USceneComponent>(TEXT("SummonSpawn0"));
	SummonSpawn0->SetupAttachment(RootComponent);
	

	SummonSpawn1 = CreateDefaultSubobject<USceneComponent>(TEXT("SummonSpawn1"));
	SummonSpawn1->SetupAttachment(RootComponent);
	

	SummonSpawn2 = CreateDefaultSubobject<USceneComponent>(TEXT("SummonSpawn2"));
	SummonSpawn2->SetupAttachment(RootComponent);
	

	SummonSpawn3 = CreateDefaultSubobject<USceneComponent>(TEXT("SummonSpawn3"));
	SummonSpawn3->SetupAttachment(RootComponent);
	
	//Laser Attack

	laserSpwan = CreateDefaultSubobject<USceneComponent>(TEXT("LaserSpwan"));
	laserSpwan->SetupAttachment(RootComponent);

	preLaserEffectSpwan = CreateDefaultSubobject<USceneComponent>(TEXT("PreLaserEffectSpwan"));
	preLaserEffectSpwan->SetupAttachment(laserSpwan);

	laserVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LaserVFX"));
	laserVFX->SetupAttachment(laserSpwan);

	//Audio
	myAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("myAudio"));
	myAudioComponent->bAutoActivate = false;
	myAudioComponent->SetupAttachment(RootComponent);
}

void AHarubus::BeginPlay()
{
	Super::BeginPlay();

	//Boss HUD
	
	addBossHUD();


	//120 Attack Timeline Binding
	if (onetwentyFloatCurve) {
		// Bind the update function
		FOnTimelineFloat onetwentyUpdateFunction;
		onetwentyUpdateFunction.BindUFunction(this, FName("onetwentyTimelineUpdate"));

		// Bind the finished function
		FOnTimelineEvent onetwentyFinishedFunction;
		onetwentyFinishedFunction.BindUFunction(this, FName("onetwentyTimelineFinished"));

		// Add functions to the timeline
		onetwentyTimeline->AddInterpFloat(onetwentyFloatCurve, onetwentyUpdateFunction);
		onetwentyTimeline->SetTimelineFinishedFunc(onetwentyFinishedFunction);

		// Set the timeline to loop or play once
		onetwentyTimeline->SetLooping(false);

	}

	//Drop Attack Timeline Binding
	if (dropAttackFloatCurve) {
		// Bind the update function
		FOnTimelineFloat dropAttackUpdateFunction;
		dropAttackUpdateFunction.BindUFunction(this, FName("dropAttackTimelineUpdate"));

		// Bind the finished function
		FOnTimelineEvent dropAttackFinishedFunction;
		dropAttackFinishedFunction.BindUFunction(this, FName("dropAttackTimelineFinished"));

		// Add functions to the timeline
		dropAttackTimeline->AddInterpFloat(dropAttackFloatCurve, dropAttackUpdateFunction);
		dropAttackTimeline->SetTimelineFinishedFunc(dropAttackFinishedFunction);

		// Set the timeline to loop or play once
		dropAttackTimeline->SetLooping(false);

	}

	//Summon Attack
	summonSpawns.Add(SummonSpawn0);
	summonSpawns.Add(SummonSpawn1);
	summonSpawns.Add(SummonSpawn2);
	summonSpawns.Add(SummonSpawn3);


	//Laser Attack
	if (laserTowerClass) {
		laserTowerRef = GetWorld()->SpawnActor<ALaserTower>(laserTowerClass, laserTowerSpwanLocation, FRotator(0,0,0));
	}

	

}

void AHarubus::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearTimer(onetwentyAttackTimer);
	GetWorldTimerManager().ClearTimer(spawnTimerHandle);
	GetWorldTimerManager().ClearTimer(startLaserAttackTimer);
	
}

void AHarubus::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!isDead && shouldFacePlayer) {
		facePlayerYaw();
	}

	if (!isDead && isLaserAttacking) {
		laserAttackTick();
	}

}

void AHarubus::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* DamageInstigator, AActor* DamageCauser)
{
	Super::DamageTaken(DamagedActor, Damage, DamageType, DamageInstigator, DamageCauser);

	updateHealthBar();

	if (isDead) HandleBossDeath();
}

void AHarubus::HandleBossDeath()
{
	//play deathsound
	if (deathSound) {
		UGameplayStatics::PlaySoundAtLocation(this, deathSound, this->GetActorLocation());
	}
	//Remove BossHUD
	if (myBossHUD) {
		myBossHUD->RemoveFromViewport();
	}

	//F Prompt
	PromptChildActorComponent->SetVisibility(true);


	//If dead in mid of Attacks
	endLaserAttack();
	laserTowerRef->Destroy();
	EndPlay(EEndPlayReason::Destroyed);
		//GetWorldTimerManager().ClearTimer(startLaserAttackTimer);
		//GetWorldTimerManager().ClearTimer(onetwentyAttackTimer);
		//GetWorldTimerManager().ClearTimer(spawnTimerHandle);

	//Stop AI
	AAIController* myAIEnemyController = getAIController();
	if (myAIEnemyController) {
		myAIEnemyController->GetBrainComponent()->StopLogic("isDead");
	}

	//Update Flipbook
	GetCharacterMovement()->StopMovementImmediately();
	//directionalFlipbooks = deathFlipbooks;
	EnemyFlipBookComponent->SetRelativeRotation(FRotator(0, -90, 0));
	EnemyFlipBookComponent->SetFlipbook(deathFlipbook);
	//EnemyFlipBookComponent->SetLooping(false);


	//Add tag
	Tags.Add(FName("ExecutableBoss"));
	

}

void AHarubus::HandleDeath()
{
	facePlayerYaw();
}

void AHarubus::executeBoss()
{

	ABGMManager* BGMManagerRef = Cast<ABGMManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ABGMManager::StaticClass())
	);

	if (BGMManagerRef) {
		BGMManagerRef->StopBGM(3);
	}

	UUserWidget* tvStaticWidget = CreateWidget<UUserWidget>(this->GetWorld(), TVStaticWidgetClass);
	if (tvStaticWidget) tvStaticWidget->AddToViewport(1);
	playerCharacter->DisableInput(Cast<APlayerController>(playerCharacter->GetController()));

	/*if (TVStaticWidgetClass) {
		FTimerHandle staticTimerHandle;
		GetWorldTimerManager().SetTimer(staticTimerHandle, [&]()
			{
				UUserWidget* tvStaticWidget = CreateWidget<UUserWidget>(this->GetWorld(), TVStaticWidgetClass);
				if (tvStaticWidget) tvStaticWidget->AddToViewport(1);
				playerCharacter->DisableInput(Cast<APlayerController>(playerCharacter->GetController()));

			}, 3, false);
	}*/


	FTimerHandle sequenceTimerHandle;
	GetWorldTimerManager().SetTimer(sequenceTimerHandle, [&]()
		{
			Destroy();
			playBossEndSequence();
		}, 1.5, false);
}

void AHarubus::playBossEndSequence()
{

	ABGMManager* BGMManagerRef = Cast<ABGMManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ABGMManager::StaticClass())
	);

	if (BGMManagerRef && BossEndSong) {
		BGMManagerRef->PlayBGM(BossEndSong);
	}



	if (bossEndSequence) {
		FMovieSceneSequencePlaybackSettings PlaybackSettings;
		PlaybackSettings.bRestoreState = false;

		ALevelSequenceActor* SequenceActor;
		ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
			GetWorld(),
			bossEndSequence,
			PlaybackSettings,
			SequenceActor
		);

		
		if (SequencePlayer && playerCharacter)
		{
			
			SequencePlayer->Play(); 
			float SequenceLength = SequencePlayer->GetDuration().AsSeconds();
			
			playerCharacter->SetActorLocation(FVector(5105.212827, -2426.513029, 2616.280702));

			playerCharacter->handleBossEnd();
			/*FTimerHandle sequenceEndTimerHandle;
			GetWorldTimerManager().SetTimer(sequenceEndTimerHandle, [&]()
				{
					OnBossEndSequenceFinished();
				}, SequenceLength, false);*/

			
		}
	
		
	
	}
}

//void AHarubus::OnBossEndSequenceFinished()
//{
//	
//	ADoomCharacter* doomCharacterRef = Cast<ADoomCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
//	if (doomCharacterRef) doomCharacterRef->resetBossEnd();
//}



void AHarubus::addBossHUD()
{
	if (bossHUDClass) {
		myBossHUD = CreateWidget<UBossHUD>(this->GetWorld(), bossHUDClass);
	}
	
	
	if (myBossHUD) {
		myBossHUD->AddToViewport();	
	}

	updateHealthBar();
}

void AHarubus::updateHealthBar()
{
	float curPercent = curHealth / maxHealth;
	if (myBossHUD) {
		myBossHUD->setBossHealthBarPercent(curPercent);
	}
}

void AHarubus::facePlayerYaw()
{
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), playerCharacter->GetActorLocation());
	this->SetActorRotation(FRotator(0, TargetRotation.Yaw, 0));
}



void AHarubus::ShootProjectle()
{
	

	GetCharacterMovement()->StopMovementImmediately();

	FVector spawnLocation1 = ProjectileSpawn->GetComponentLocation();
	FRotator spawnRotation1 = ProjectileSpawn->GetComponentRotation();
	


	FVector spawnLocation2 = ProjectileSpawn2->GetComponentLocation();
	FRotator spawnRotation2 = ProjectileSpawn2->GetComponentRotation();
	



	if (ProjectileClass) {
		ABaseProjectile* Projectile1 = GetWorld()->SpawnActor<ABaseProjectile>(ProjectileClass, spawnLocation1, spawnRotation1);
		if (Projectile1) {
			Projectile1->SetOwner(this);

		}

		ABaseProjectile* Projectile2 = GetWorld()->SpawnActor<ABaseProjectile>(ProjectileClass, spawnLocation2, spawnRotation2);
		if (Projectile2) {
			Projectile2->SetOwner(this);

		}

		if (projectileSound) {
			UGameplayStatics::PlaySoundAtLocation(this, projectileSound, this->GetActorLocation());
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Empty ProjectileClass"));
	}



}

void AHarubus::onetwentyAttack()
{
	facePlayerYaw();
	isAttacking = true;
	attackingstate = RangedAttacking;

	GetWorldTimerManager().SetTimer(onetwentyAttackTimer, [&]()
		{
			ShootProjectle();
		}, 0.1, true, 0);


	FRotator currRotation = GetActorRotation();

	double lookatRotationPitch = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), playerCharacter->GetActorLocation()).Pitch;

	if (FMath::RandRange(0, 1) == 0) {
		onetwentyStart = FRotator(lookatRotationPitch, currRotation.Yaw - 60, currRotation.Roll);
		onetwentyEnd = FRotator(lookatRotationPitch, currRotation.Yaw + 60, currRotation.Roll);
	}
	else {
		onetwentyStart = FRotator(lookatRotationPitch, currRotation.Yaw + 60, currRotation.Roll);
		onetwentyEnd = FRotator(lookatRotationPitch, currRotation.Yaw - 60, currRotation.Roll);
	}

	onetwentyTimeline->PlayFromStart();
}



void AHarubus::onetwentyTimelineUpdate(float Value)
{
	
	FRotator newRoation = FMath::Lerp(onetwentyStart, onetwentyEnd, Value);
	this->SetActorRotation(newRoation);

}

void AHarubus::onetwentyTimelineFinished()
{
	GetWorldTimerManager().ClearTimer(onetwentyAttackTimer);
	isAttacking = false;
	this->SetActorRotation(FRotator(0, GetActorRotation().Yaw, GetActorRotation().Roll));
}


void AHarubus::startPreDropAttack()
{
	// isInvin off when finish drop attack
	isInvin = true;

	disableGravity();

	EnemyFlipBookComponent->SetVisibility(false);

	if (preDropFlipbook) {
		AnimationFlipBookComponent->SetFlipbook(preDropFlipbook);

		AnimationFlipBookComponent->SetRelativeLocation(FVector(0,0,40));
		AnimationFlipBookComponent->SetRelativeScale3D(FVector(9, 9, 9));

		AnimationFlipBookComponent->PlayFromStart();
	}

	if (preDropSound) {
		UGameplayStatics::PlaySoundAtLocation(this, preDropSound, this->GetActorLocation());
	}


	//Disappear after flame
	FTimerHandle disappearTimerHandle;
	GetWorldTimerManager().SetTimer(disappearTimerHandle, [&]()
		{
			UE_LOG(LogTemp, Display, TEXT("disappearTimerHandle"));
			this->SetActorLocation(GetActorLocation() + FVector(0, 0, 1000));
		}, AnimationFlipBookComponent->GetFlipbookLength(), false);


	//Determine drop location after dropAttackDelay
	FTimerHandle startDropTimerHandle;

	GetWorldTimerManager().SetTimer(startDropTimerHandle, [&]()
		{
			UE_LOG(LogTemp, Display, TEXT("startDropTimerHandle"));
			startDropAttack();
		}, dropAttackDelay, false);

}

void AHarubus::startDropAttack()
{
	shouldUpdateDirectionalSprite = false;

	setCollisionOverlapPawn();

	EnemyFlipBookComponent->SetVisibility(true);
	EnemyFlipBookComponent->SetFlipbook(directionalFlipbooks[0]);

	dropAttackEndLocation = playerCharacter->GetActorLocation();
	dropAttackStartLocation = dropAttackEndLocation + FVector(0, 0, 1000);

	this->SetActorLocation(dropAttackStartLocation);

	if (dropAttackIndicator) {
		GetWorld()->SpawnActor<AActor>(dropAttackIndicator, dropAttackEndLocation, FRotator(0,0,0));
	}

	if (midDropSound) {
		UGameplayStatics::PlaySoundAtLocation(this, midDropSound, this->GetActorLocation());
	}
	
	//give player time to run away from the red circle
	FTimerHandle actualDropTimerHandle;

	GetWorldTimerManager().SetTimer(actualDropTimerHandle, [&]()
		{
			EnemyFlipBookComponent->SetRelativeRotation(FRotator(0, -90, 0));
			dropAttackTimeline->PlayFromStart();
		}, dropAttackIndicatorTime, false);
	

}

void AHarubus::dropAttackTimelineUpdate(float Value)
{
	facePlayerYaw();

	FVector newLocation = FMath::Lerp(dropAttackStartLocation, dropAttackEndLocation + FVector(0, 0, 150), Value);
	this->SetActorLocation(newLocation);

}

void AHarubus::dropAttackTimelineFinished()
{
	dropAttackSphereTrace();
	isInvin = false;
	shouldUpdateDirectionalSprite = true;
	enableGravity();

	if (dropAttackLandEffect) {
		FVector effectLocation = dropAttackEndLocation + FVector(0, 0, -100);

		GetWorld()->SpawnActor<AActor>(dropAttackLandEffect, effectLocation, FRotator(0, 0, 0));
	}

	if (finalDropSound) {
		UGameplayStatics::PlaySoundAtLocation(this, finalDropSound, this->GetActorLocation());
	}


	
	FTimerHandle resetCollisionTimerHandle;

	GetWorldTimerManager().SetTimer(resetCollisionTimerHandle, [&]()
		{
			setCollisionBlockPawn();
		}, 0.2, false);
	


}

void AHarubus::disableGravity()
{
	GetCharacterMovement()->GravityScale = 0;
}

void AHarubus::enableGravity()
{
	GetCharacterMovement()->GravityScale = 1;
}

void AHarubus::setCollisionOverlapPawn()
{
	if (myCapsuleComponent) {
		myCapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
}

void AHarubus::setCollisionBlockPawn()
{
	if (myCapsuleComponent) {
		myCapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	}
}



void AHarubus::dropAttackSphereTrace()
{
	TArray<FHitResult> HitResults;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_Pawn) };
	TArray<AActor*> ActorsToIgnore = { Cast<AActor>(this) };

	bool hasHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		this->GetWorld(),
		dropAttackEndLocation,
		dropAttackEndLocation,
		dropAttackRange,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::Type::None,
		HitResults,
		true
	);


	if (hasHit) {
		for (FHitResult hitResult : HitResults) {
			AActor* HitActor = hitResult.GetActor();
			AController* MyInstigator = GetInstigatorController();
			auto DamageTypeClass = UDamageType::StaticClass();
			if (HitActor->ActorHasTag("Player")) {
				UGameplayStatics::ApplyDamage(HitActor, dropAttackDamage, MyInstigator, this, DamageTypeClass);

				FVector launchDirection;
				
				if (playerCharacter->GetActorLocation() != dropAttackEndLocation) {
					launchDirection = playerCharacter->GetActorLocation() - dropAttackEndLocation;
				}
				else {
					launchDirection = -playerCharacter->GetActorForwardVector();
				}
				
				
				launchDirection.Normalize();


				FVector launchVelocity = FVector(launchDirection.X * 10000, launchDirection.Y * 10000, 0);

				playerCharacter->LaunchCharacter(launchVelocity, false, false);
			}
		}

	}
}

void AHarubus::dropAttack()
{
	facePlayerYaw();
	startPreDropAttack();

}




void AHarubus::spawnSelfDes()
{

	if (spawnCount < 4) {
		USceneComponent* summonSpawn = summonSpawns[spawnCount];
		FVector spawnLocation = summonSpawn->GetComponentLocation();
		FRotator spawnRotation = summonSpawn->GetComponentRotation();

		
		ASelfDestructEnemy* selfDesEnemy = GetWorld()->SpawnActor<ASelfDestructEnemy>(SelfDesEnemyClass, spawnLocation, spawnRotation);
		if (selfDesEnemy) {
			selfDesEnemy->setCanSeePlayer(true);
			selfDesEnemy->setBlackBoardCanSeePlayer(true);
		}
		


		spawnCount++;
	}
	else {
		GetWorldTimerManager().ClearTimer(spawnTimerHandle);
		shouldFacePlayer = false;
	}




}

void AHarubus::startSummonAttack()
{
	if (SelfDesEnemyFlameClass) {
		for (USceneComponent* summonSpawn : summonSpawns) {
			FVector spawnLocation = summonSpawn->GetComponentLocation();
			FRotator spawnRotation = summonSpawn->GetComponentRotation();
			GetWorld()->SpawnActor<AActor>(SelfDesEnemyFlameClass, spawnLocation, spawnRotation);
		}
	}



	if (SelfDesEnemyClass) {
		spawnCount = 0;
		GetWorldTimerManager().SetTimer(spawnTimerHandle, this, &AHarubus::spawnSelfDes, SpawnInterval, true);

	}
}



void AHarubus::summonAttack()
{
	facePlayerYaw();
	//shouldFacePlayer = true;

	if (summonAttackEffect) {
		FVector effectLocation = GetActorLocation() + FVector(0, 0, -240);

		GetWorld()->SpawnActor<AActor>(summonAttackEffect, effectLocation, FRotator(0, 0, 0));
	}
	
	if (summonSound) {
		UGameplayStatics::PlaySoundAtLocation(this, summonSound, this->GetActorLocation());
	}

	FTimerHandle startAttackTimer;

	GetWorldTimerManager().SetTimer(startAttackTimer, [&]() {
		startSummonAttack();
		}, 2.5, false);


	

}




void AHarubus::laserAttack()
{

	facePlayerYaw();
	shouldFacePlayer = true;
	

	//Play Pre FX
	if (preLaserAttackEffect) {
		GetWorld()->SpawnActor<AActor>(preLaserAttackEffect, preLaserEffectSpwan->GetComponentLocation(), FRotator(0, 0, 0));
	}


	//Activate Tower
	if (laserTowerRef) {
		laserTowerRef->activateTower();
	}
	
	if (laserStartSound) {
		UGameplayStatics::PlaySoundAtLocation(this, laserStartSound, this->GetActorLocation());
	}

	

	GetWorldTimerManager().SetTimer(startLaserAttackTimer, [&]() {
		//Start Self Laser
		laserVFX->SetVisibility(true);
		isLaserAttacking = true;
		shouldUpdateDirectionalSprite = false;
		if (laserAttackFlipbook) {
			EnemyFlipBookComponent->SetFlipbook(laserAttackFlipbook);
		
		}

		if (laserAttackSound && myAudioComponent) {
			myAudioComponent->SetSound(laserAttackSound);
			myAudioComponent->Play();
		}

	}, 3, false);
	
	


}

void AHarubus::endLaserAttack()
{
	if (laserTowerRef) {
		laserTowerRef->deactivateTower();
	}

	//End Self Laser
	laserVFX->SetVisibility(false);
	isLaserAttacking = false;
	shouldFacePlayer = false;
	shouldUpdateDirectionalSprite = true;

	if (myAudioComponent) {
		myAudioComponent->Stop();
	}

}



void AHarubus::laserAttackTick()
{

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_WorldStatic),
		UEngineTypes::ConvertToObjectType(ECC_WorldDynamic),
		UEngineTypes::ConvertToObjectType(ECC_Pawn),
		UEngineTypes::ConvertToObjectType(ECC_Destructible)
	};

	TArray<AActor*> ActorsToIgnore = { Cast<AActor>(this) };

	FVector laserStart = laserSpwan->GetComponentLocation();
	FVector laserEnd = playerCharacter->GetActorLocation();


	FHitResult HitResult;

	bool hasHit = UKismetSystemLibrary::LineTraceSingleForObjects(this->GetWorld(),
		laserStart,
		laserEnd,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::Type::None,
		HitResult,
		true);

	if (hasHit) {
		laserVFX->SetVectorParameter("Beam End", HitResult.Location);

		AActor* HitActor = HitResult.GetActor();

		if (HitActor->ActorHasTag("Player") && canLaserHurt) {
			UGameplayStatics::ApplyDamage(HitActor, laserDamage, GetInstigatorController(), this, UDamageType::StaticClass());
			laserHasHurt();
		}

	}
}

void AHarubus::laserHasHurt()
{
	GetWorldTimerManager().ClearTimer(laserHurtTimerHandle);
	canLaserHurt = false;

	GetWorld()->GetTimerManager().SetTimer(laserHurtTimerHandle, [&]()
		{
			canLaserHurt = true;
		}, laserHurtInterval, false);
}


void AHarubus::risingSunAttack()
{
	facePlayerYaw();

	risingSunSpawnLocation = GetActorLocation() + GetActorForwardVector() * 100 - FVector(0, 0, 400);


	if (risingProjectileClass) {
		GetWorld()->SpawnActor<AActor>(risingProjectileClass, risingSunSpawnLocation, FRotator(0, 180, 0));

		FTimerHandle leftrightTimerHandle;

		GetWorldTimerManager().SetTimer(leftrightTimerHandle, [&]() {
			GetWorld()->SpawnActor<AActor>(risingProjectileClass, risingSunSpawnLocation - GetActorRightVector() * 600, FRotator(0, 180, 0));
			GetWorld()->SpawnActor<AActor>(risingProjectileClass, risingSunSpawnLocation + GetActorRightVector() * 600, FRotator(0, 180, 0));

			}
		, 0.3, false);


	}

	if (sunAttackSound) {
		UGameplayStatics::PlaySoundAtLocation(this, sunAttackSound, this->GetActorLocation());
	}
}
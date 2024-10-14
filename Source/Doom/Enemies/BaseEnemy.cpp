// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "Doom/DoomCharacter.h"
#include "Kismet/GameplayStatics.h"
#include <Kismet/KismetMathLibrary.h>
#include "Doom/Projectile/BaseProjectile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "BehaviorTree/BlackboardComponent.h"



// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	EnemyFlipBookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("EnemyFlipBook"));
	EnemyFlipBookComponent->SetupAttachment(RootComponent);
	
	ProjectileSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawn"));
	ProjectileSpawn->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	playerCharacter = Cast<ADoomCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	gameStateRef = GetWorld()->GetGameState<ADoomGameStateBase>();

	currentFlipbooks = directionalFlipbooks;

	OnTakeAnyDamage.AddDynamic(this, &ABaseEnemy::DamageTaken);
	
}

void ABaseEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearTimer(attackingTimerHandle);
	GetWorldTimerManager().ClearTimer(deathTimerHandle);
	GetWorldTimerManager().ClearTimer(attackWindowTimerHandle);
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckEnemyState();

	/*if (canSeePlayer && !isDead) {
		rotateToPlayer(DeltaTime);
	}*/

	if (isDead) {
		HandleDeath();
		rotateToPlayer(DeltaTime);
	}
	else {
		updateDirectionalSprite();
	}
	
	
}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}



AAIController* ABaseEnemy::getAIController()
{
	AController* myEnemyController = this->GetController();
	AAIController* myAIEnemyController;
	if (myEnemyController) {
		myAIEnemyController = Cast<AAIController>(myEnemyController);
		if (myAIEnemyController) return myAIEnemyController;
		
	}
	return nullptr;
}

void ABaseEnemy::updateDirectionalSprite()
{
	


	FVector vEnemeyPlayer = playerCharacter->GetActorLocation() - GetActorLocation();
	vEnemeyPlayer.Normalize();

	//Enemy->Player dot Enemy Forward vector
	float EPdotEF = FVector::DotProduct(vEnemeyPlayer, GetActorForwardVector());

	//Degree with regard to Enemy Forward Vector
	float radians = FMath::Acos(EPdotEF);
	float degrees = FMath::RadiansToDegrees(radians);

	//Enemy->Player dot Enemy Right vector
	//if EPdotER > 0, player is on the left side of Enemy, enemy show front left. EPdotER < 0 means player is on the righ
	float EPdotER = FVector::DotProduct(vEnemeyPlayer, GetActorRightVector());


	if (EPdotER > 0) {
		if (degrees >= 0 && degrees < 20) {
			updateFlipbook(-90, 0);
		}
		else if (degrees >= 20 && degrees < 70) {
			updateFlipbook(-45, 1);
		}
		else if (degrees >= 70 && degrees < 120) {
			updateFlipbook(0, 2);
		}
		else if (degrees >= 120 && degrees < 160) {
			updateFlipbook(45, 3);
		}
		else {
			updateFlipbook(90, 4);
		}
	}
	else {
		if (degrees >= 0 && degrees < 20) {
			updateFlipbook(-90, 0);
		}
		else if (degrees >= 20 && degrees < 70) {
			updateFlipbook(-135, 7);
		}
		else if (degrees >= 70 && degrees < 120) {
			updateFlipbook(180, 6);
		}
		else if (degrees >= 120 && degrees < 160) {
			updateFlipbook(135, 5);
		}
		else {
			updateFlipbook(90, 4);
		}
	}


	//UE_LOG(LogTemp, Warning, TEXT("Dot Product: %f"), degrees);



}

void ABaseEnemy::updateFlipbook(float degree, int32 index)
{
	switch (enemyState)
	{
	case IdleState:
		currentFlipbooks = directionalFlipbooks;
		break;

	case MovingState:
		currentFlipbooks = movingFlipbooks;
		break;

	case PreMeleeAttackState:
		currentFlipbooks = preMeleeAttackFlipbooks;
		break;

	case MeleeAttackState:
		currentFlipbooks = meleeAttackFlipbooks;
		break;

	case RangedAttackState:
		currentFlipbooks = rangedAttackFlipbooks;
		break;

	default:
		break;
	}


	EnemyFlipBookComponent->SetRelativeRotation(FRotator(0, degree, 0));
	EnemyFlipBookComponent->SetFlipbook(currentFlipbooks[index]);
}

void ABaseEnemy::rotateToPlayer(float DeltaTime)
{

	
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), playerCharacter->GetActorLocation());

	FRotator NewRotation = FMath::RInterpConstantTo(this->GetActorRotation(), TargetRotation, DeltaTime, rotationSpeed);

	//this->SetActorRotation(NewRotation);

	this->SetActorRotation(FRotator(0, NewRotation.Yaw,0));


}

void ABaseEnemy::CheckEnemyState()
{
	if (GetVelocity().Size() > 0) {
		if (isAttacking) {
			switch (attackingstate) {
				case PreMeleeAttacking:
					enemyState = PreMeleeAttackState;
					break;

				case MeleeAttacking:
					enemyState = MeleeAttackState;
					break;

				case RangedAttacking:
					enemyState = RangedAttackState;
					break;
			}
		}
		else {
			enemyState = MovingState;
		}
	}
	else {
		if (isAttacking) {
			switch (attackingstate) {
			case PreMeleeAttacking:
				enemyState = PreMeleeAttackState;
				break;

			case MeleeAttacking:
				enemyState = MeleeAttackState;
				break;

			case RangedAttacking:
				enemyState = RangedAttackState;
				break;
			}
		}
		else {
			enemyState = IdleState;
		}
	}
}

void ABaseEnemy::ShootProjectle()
{
	isAttacking = true;
	attackingstate = RangedAttacking;

	GetCharacterMovement()->StopMovementImmediately();

	FVector spawnLocation = ProjectileSpawn->GetComponentLocation();
	//FRotator spawnRotation = ProjectileSpawn->GetComponentRotation();
	FVector playerLocation = playerCharacter->GetActorLocation();
	FRotator spawnRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), FVector(playerLocation.X, playerLocation.Y, playerLocation.Z + 50));

	if (ProjectileClass) {
		ABaseProjectile* Projectile = GetWorld()->SpawnActor<ABaseProjectile>(ProjectileClass, spawnLocation, spawnRotation);
		//ABaseProjectile* Projectile = GetWorld()->SpawnActorDeferred<ABaseProjectile>(ProjectileClass, SpawnTransform);
		if (Projectile) {

			//Projectile->projectileDamage = weaponDamage;
			Projectile->SetOwner(this);

		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Empty ProjectileClass"));
	}


	GetWorld()->GetTimerManager().ClearTimer(attackingTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(attackingTimerHandle, [&]()
		{
			isAttacking = false;
		}, 0.5, false);
}

void ABaseEnemy::MeleeAttack()
{
	isAttacking = true;
	attackingstate = PreMeleeAttacking;

	float Distance = FVector::Dist(this->GetActorLocation(), playerCharacter->GetActorLocation());
	if (Distance <= meleeAttackRange) {
		curAttackInfo.StartTime = GetWorld()->GetTimeSeconds();
		curAttackInfo.Duration = meleeAttackWindow;
		curAttackInfo.Attacker = this;

		gameStateRef->addAttack(curAttackInfo);
		isAdded = true;
	}

	//give a attack window delay for perfect dodge
	GetWorld()->GetTimerManager().SetTimer(attackWindowTimerHandle, [&]()
	{
			
			attackingstate = MeleeAttacking;

			FVector lineTraceLocation = this->GetActorLocation();
			FVector lineTraceForward = this->GetActorForwardVector();
			FVector lineTraceEnd = lineTraceForward * meleeAttackRange + lineTraceLocation;
			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_Pawn) };

			TArray<AActor*> ActorsToIgnore = { Cast<AActor>(this) };
			FHitResult HitResult;

			//Line Trace
			bool hasHit = UKismetSystemLibrary::LineTraceSingleForObjects(this->GetWorld(),
				lineTraceLocation,
				lineTraceEnd,
				ObjectTypes,
				false,
				ActorsToIgnore,
				EDrawDebugTrace::Type::ForDuration,
				HitResult,
				true);

			//Apply DMG
			if (hasHit) {
				AActor* HitActor = HitResult.GetActor();

				if (HitActor == UGameplayStatics::GetPlayerCharacter(this, 0)) {
					//AActor* myOwner = GetOwner();
					AController* MyOwnerInstigator = GetInstigatorController();
					auto DamageTypeClass = UDamageType::StaticClass();
					UGameplayStatics::ApplyDamage(HitActor, meleeDamage, MyOwnerInstigator, this, DamageTypeClass);
				}
			}

			GetWorld()->GetTimerManager().ClearTimer(attackingTimerHandle);
			GetWorld()->GetTimerManager().SetTimer(attackingTimerHandle, [&]()
				{
					isAttacking = false;
					if (isAdded) gameStateRef->removeAttack(curAttackInfo);

				}, 0.5, false);

	}, meleeAttackWindow, false);



	

	
}

void ABaseEnemy::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* DamageInstigator, AActor* DamageCauser)
{
	curHealth -= Damage;
	UE_LOG(LogTemp, Display, TEXT("Heath: %f"), curHealth);

	if (curHealth <= 0) {
		isDead = true;
	}

	//Sense player when damaged
	AAIController* myAIEnemyController = getAIController();
	if (myAIEnemyController) {
		UBlackboardComponent* MyBlackboard = myAIEnemyController->GetBlackboardComponent();
		canSeePlayer = true;
		if (MyBlackboard) MyBlackboard->SetValueAsBool(FName("CanSeePlayer"), true);
	}
	
	
}

void ABaseEnemy::HandleDeath() {
	
	

	//Face Player
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), playerCharacter->GetActorLocation());
	this->SetActorRotation(TargetRotation);


	if (isDying) return;

	//Stop AI
	AAIController* myAIEnemyController = getAIController();
	if (myAIEnemyController) {
		myAIEnemyController->GetBrainComponent()->StopLogic("isDead");
	}
		
	


	//Update Flipbook
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->StopMovementImmediately();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EnemyFlipBookComponent->SetRelativeRotation(FRotator(0, -90, 0));
	EnemyFlipBookComponent->SetFlipbook(deathFlipbook);
	EnemyFlipBookComponent->SetLooping(false);
	
	
	GetWorld()->GetTimerManager().SetTimer(deathTimerHandle, [&]()
		{
			Destroy();
		}, 3, false);

	isDying = true;
}
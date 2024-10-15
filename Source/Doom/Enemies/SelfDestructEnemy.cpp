// Fill out your copyright notice in the Description page of Project Settings.


#include "SelfDestructEnemy.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "Doom/DoomCharacter.h"
#include "Kismet/GameplayStatics.h"
#include <Kismet/KismetMathLibrary.h>
#include "Doom/Projectile/BaseProjectile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"

ASelfDestructEnemy::ASelfDestructEnemy()
{
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));

	if (PawnSensingComponent)
	{
		PawnSensingComponent->SightRadius = 1500.0f;
		PawnSensingComponent->HearingThreshold = 600.0f;
		PawnSensingComponent->LOSHearingThreshold = 1200.0f;
		PawnSensingComponent->bHearNoises = true;
	}
}

void ASelfDestructEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (PawnSensingComponent) {
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &ASelfDestructEnemy::OnPawnSeen);
		//PawnSensingComponent->OnHearNoise.AddDynamic(this, &ASelfDestructEnemy::OnNoiseHeard);
	
	}

	myAIController = getAIController();
	if (myAIController && myBehaviorTree) {
		myAIController->RunBehaviorTree(myBehaviorTree);
	}
	
}

void ASelfDestructEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearTimer(selfDesTimerHandle);
	GetWorldTimerManager().ClearTimer(sphereTraceTimerHandle);
	GetWorldTimerManager().ClearTimer(blinkEffectTimerHandle);
	GetWorldTimerManager().ClearTimer(resetCanSeePlayerTimerHandle);
}


void ASelfDestructEnemy::OnPawnSeen(APawn* SeenPawn)
{
	ADoomCharacter* playerRef = Cast<ADoomCharacter>(SeenPawn);
	if (playerRef) {
		if (playerRef->getIsAlive()) {
			setCanSeePlayer(true);
			setBlackBoardCanSeePlayer(true);
		
		}
		else {
			resetCanSeePlayer();
			setBlackBoardCanSeePlayer(false);
		}

		//If can't sense player after 5s, go back to patrol, add clearTimer to make it a retriggerable delay
		GetWorldTimerManager().ClearTimer(resetCanSeePlayerTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(resetCanSeePlayerTimerHandle, [&]() {
			resetCanSeePlayer();
			setBlackBoardCanSeePlayer(false);
			}, 5, false);
	}
}

	

void ASelfDestructEnemy::setBlackBoardCanSeePlayer(bool value)
{
	
	if (myAIController) {
		UBlackboardComponent* BlackboardComp = myAIController->GetBlackboardComponent();
		if (BlackboardComp) {
			BlackboardComp->SetValueAsBool(TEXT("CanSeePlayer"), value);
		}
	}
}

void ASelfDestructEnemy::selfDestruct()
{
	//no longer takes dmg if self des started
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->StopMovementImmediately();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AAIController* myAIEnemyController = getAIController();
	if (myAIEnemyController) {
		myAIEnemyController->GetBrainComponent()->StopLogic("isDead");
	}

	//start blinking
	isAttacking = true;
	attackingstate = MeleeAttacking;

	GetWorld()->GetTimerManager().SetTimer(blinkEffectTimerHandle, [&]() {
		if (isRed) {
			EnemyFlipBookComponent->SetSpriteColor(FLinearColor::White);
			isRed = false;
		}
		else {
			EnemyFlipBookComponent->SetSpriteColor(FLinearColor::Red);
			isRed = true;
		}
		
		}, blinkRate, true, 0);

	
		
	GetWorld()->GetTimerManager().SetTimer(selfDesTimerHandle, [&]()
		{

		//self destruct needs to be warped under another time handle to handle the actual self destruct after blinking started
			//Actual self destruct
			//Add Attack info
			float Distance = FVector::Dist(this->GetActorLocation(), playerCharacter->GetActorLocation());
			if (Distance <= destructRadius) {
				curAttackInfo.StartTime = GetWorld()->GetTimeSeconds();
				curAttackInfo.Duration = destructDodgeWindow;
				curAttackInfo.Attacker = this;

				gameStateRef->addAttack(curAttackInfo);
				isAdded = true;
			}

			//draw trace after dodge window
			GetWorld()->GetTimerManager().SetTimer(sphereTraceTimerHandle, [&]()
				{
					GetWorld()->GetTimerManager().ClearTimer(blinkEffectTimerHandle);

					TArray<FHitResult> HitResults;
					FVector actorLocation = this->GetActorLocation();
					TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_Pawn) };
					TArray<AActor*> ActorsToIgnore = { Cast<AActor>(this) };

					bool hasHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
						this->GetWorld(),
						actorLocation,
						actorLocation,
						destructRadius,
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


							if (HitActor->ActorHasTag("Player")) {
								UGameplayStatics::ApplyDamage(HitActor, destructDamage, MyInstigator, this, DamageTypeClass);

							}
						}
					}

					if (isAdded) gameStateRef->removeAttack(curAttackInfo);

					isDead = true;
					isSelfDes = true;

				}, destructDodgeWindow, false);

		}, destructTime, false);
		
	
}

void ASelfDestructEnemy::HandleDeath() 
{

	if (isSelfDes) {
		

		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), playerCharacter->GetActorLocation());
		this->SetActorRotation(TargetRotation);

		if (isDestructing) return;

		//Stop AI
		AAIController* myAIEnemyController = getAIController();
		if (myAIEnemyController) {
			myAIEnemyController->GetBrainComponent()->StopLogic("isDead");
		}




		//Update Flipbook
	/*	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		GetCharacterMovement()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);*/

		EnemyFlipBookComponent->SetRelativeRotation(FRotator(0, -90, 0));
		EnemyFlipBookComponent->SetWorldScale3D(FVector(25, 25, 25));
		//EnemyFlipBookComponent->SetRelativeLocation(FVector(15, 15, 15));
		
		EnemyFlipBookComponent->SetFlipbook(selfDesFlipbook);
		
		//EnemyFlipBookComponent->SetLooping(false);


		GetWorld()->GetTimerManager().SetTimer(deathTimerHandle, [&]()
			{
				Destroy();
			}, 1, false);

		isDestructing = true;
	}
	else {
		Super::HandleDeath();
	}
	
}



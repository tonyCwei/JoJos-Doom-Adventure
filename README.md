# JoJo's Doom Adventure  

Source Code of JoJo's Doom Adventure with Unreal Engine 5  

## 🎮 Gameplay Video
[![Watch the gameplay](https://img.youtube.com/vi/dAzUW5b5_-w/0.jpg)](https://youtu.be/dAzUW5b5_-w)  
*Click the image above to watch the full gameplay video on YouTube*



## 🕹️ Play the Game
[![itch.io Version](https://img.shields.io/badge/Download%20on-itch.io-FA5C5C.svg)](https://frigidough.itch.io/jojos-doom-adventure)  


## 📘 Development Log
[View Development Progress](https://docs.google.com/spreadsheets/d/1NHgv4hAetz6-0AA-LgOZ9bWJvBuZY5fXOvtN7yXrdTY/edit#gid=0)  

## 🚀 Technical Highlights  

### Eight-Directional Sprite System  
The game features an eight-directional sprite system similar to the classic *Doom (1993)*. 
Enemies dynamically change their sprites based on their position relative to the player, 
creating a sense of being a 3D object while only using sprites.  

[![Eight-Directional Sprite System](https://media2.giphy.com/media/v1.Y2lkPTc5MGI3NjExYmdzNXJpODJ2MHptODd5eTI3ajVvZW9ueGxkdWliM2w5bDVxeG1ueiZlcD12MV9pbnRlcm5naWZfYnlfaWQmY3Q9Zw/gZdBe1mF5ISCfxP2To/giphy.gif)](https://media2.giphy.com/media/v1.Y2lkPTc5MGI3NjExYmdzNXJpODJ2MHptODd5eTI3ajVvZW9ueGxkdWliM2w5bDVxeG1ueiZlcD12MV9pbnRlcm5naWZfYnlfaWQmY3Q9Zw/gZdBe1mF5ISCfxP2To/giphy.gif)  

The system determines the player's relative position using vector calculations:  

- **Enemy Forward Vector**  
- **Enemy Right Vector** 
- **Vector from Enemy to Player** (`Enemy->Player`)  

1. Use dot product of `Enemy->Player` and `Enemy Forward Vector` and convert to degrees to determine if the player is in front or behind.  

2. Use dot product of `Enemy->Player` and `Enemy Right Vector` to determine whether the player is on the left or right.  

```cpp
void ABaseEnemy::updateDirectionalSprite()
{
	if (!playerCharacter) return;
	
	FVector vEnemeyPlayer = playerCharacter->GetActorLocation() - GetActorLocation();
	vEnemeyPlayer.Normalize();

	//Enemy->Player dot Enemy Forward vector
	float EPdotEF = FVector::DotProduct(vEnemeyPlayer, GetActorForwardVector());

	//Degree between Enemy->Player and Enemy Forward Vector
	float radians = FMath::Acos(EPdotEF);
	float degrees = FMath::RadiansToDegrees(radians);

	//Enemy->Player dot Enemy Right vector
	//if EPdotER > 0, player is on the Enemy Right. EPdotER < 0 means player is on the enemy left
	float EPdotER = FVector::DotProduct(vEnemeyPlayer, GetActorRightVector());

	if (EPdotER > 0) {
		if (degrees >= 0 && degrees < 20) {
			//Show enemy front sprite
			updateFlipbook(-90, 0);
		}
		else if (degrees >= 20 && degrees < 70) {
			//Show enemy front right sprite
			updateFlipbook(-45, 1);
		}
		else if (degrees >= 70 && degrees < 120) {
			//Show enemy right sprite
			updateFlipbook(0, 2);
		}
		else if (degrees >= 120 && degrees < 160) {
			//Show enemy back right sprite
			updateFlipbook(45, 3);
		}
		else {
			//Show enemy back sprite
			updateFlipbook(90, 4);
		}
	}
	else {
		if (degrees >= 0 && degrees < 20) {
			//Show enemy front sprite
			updateFlipbook(-90, 0);
		}
		else if (degrees >= 20 && degrees < 70) {
			//Show enemy front left sprite
			updateFlipbook(-135, 7);
		}
		else if (degrees >= 70 && degrees < 120) {
			//Show enemy left sprite
			updateFlipbook(180, 6);
		}
		else if (degrees >= 120 && degrees < 160) {
			//Show enemy back left sprite
			updateFlipbook(135, 5);
		}
		else {
			//Show enemy back sprite
			updateFlipbook(90, 4);
		}
	}
}
```

`updateFlipbook` is a helper function that updates the flipbook array based on an enum called `enemyState`. 
The `relativeDegree` parameter is used to set the relative rotation of the `EnemyFlipBookComponent`, ensuring it always faces the player.  

```cpp
void ABaseEnemy::updateFlipbook(float relativeDegree, int32 index)
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

	EnemyFlipBookComponent->SetRelativeRotation(FRotator(0, relativeDegree, 0));
	EnemyFlipBookComponent->SetFlipbook(currentFlipbooks[index]);
}
```

### Perfect Dodge System 
The game features a *Perfect Dodge* system where the player can slow down time if they dodge an enemy attack (either melee or ranged) in the last second. 
During a perfect dodge, the environment will shift to black and white, and time will slow around the player, allowing them to take more actions.
While time slows for the enemies and environment, the player themselves remain unaffected by the time slowdown.

Melee Attack Perfect Dodge:  
  
[![Melee Attack Perfect Dodge](https://media.giphy.com/media/HJYw7YHBabJ1qHLwVH/giphy.gif)](https://media.giphy.com/media/HJYw7YHBabJ1qHLwVH/giphy.gif)

Enemy Projectile Perfect Dodge:  
  
[![Enemy Projectile Perfect Dodge](https://media.giphy.com/media/pXhK0TDWxcG7Cb5F5w/giphy.gif)](https://media.giphy.com/media/pXhK0TDWxcG7Cb5F5w/giphy.gif)

The *Perfect Dodge* system consists of two main parts:  

1. **Determining if the Player's Dodge Was Perfect**  
2. **Slowing Down Time Around the Player and Activating the Black-and-White Effect**  

#### Part 1: Perfect Dodge Check  
A struct called `FAttackInfo` is created in the custom GameState Class `ADoomGameStateBase`. This struct stores information about each attack, such as the start time, duration, and the attacker.

```cpp
struct FAttackInfo
{
	GENERATED_BODY()

	float StartTime;
	float Duration;
	AActor* Attacker;

	bool operator==(const FAttackInfo& Other) const
	{
		return (StartTime == Other.StartTime &&
			Duration == Other.Duration &&
			Attacker == Other.Attacker);
	}
};
```

Each time an enemy melee attacks, the addMeleeAttackInfo function is called. 
This function checks if the player is within melee attack range, and if true, it updates the AttackInfo and adds it to the activeAttacks array in the game state.

```cpp
void ABaseEnemy::addMeleeAttackInfo()
{
	float Distance = FVector::Dist(this->GetActorLocation(), playerCharacter->GetActorLocation());
	if (Distance <= meleeAttackRange) { // If player is within the melee attack range
		curAttackInfo.StartTime = GetWorld()->GetTimeSeconds();
		curAttackInfo.Duration = meleeAttackDodgeWindow;
		curAttackInfo.Attacker = this;
		gameStateRef->addAttack(curAttackInfo);
		isAdded = true;
	}
}
```

As for enemy projectiles, since there’s no attack range check like melee attacks, 
a different method is used to determine whether the player is going to be hit by the projectile. 
To accomplish this, a trigger box is added to each enemy projectile. 
This trigger box is triggered when it overlaps the player, which is when the dodge mechanic can be triggered.

![Enemy Projectile Trigger Box](https://i.imgur.com/CEuTxoO.png)
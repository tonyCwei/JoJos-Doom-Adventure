# JoJo's Doom Adventure  

Source Code of JoJo's Doom Adventure with Unreal Engine 5  
  
[![itch.io Version](https://img.shields.io/badge/Download%20on-itch.io-FA5C5C.svg?logoWidth=150)](https://frigidough.itch.io/jojos-doom-adventure)  


## 🎮 Gameplay Video
<p align="center">
  <a href="https://youtu.be/dAzUW5b5_-w">
    <img src="https://img.youtube.com/vi/dAzUW5b5_-w/0.jpg" alt="Watch the gameplay">
  </a>
  <br>
  <em>Click the image above to watch the full gameplay video with detailed timestamps on YouTube</em>
</p>

## 🚀 Highlights  

- [Level Design](#level-design)
- [Perfect Dodge System](#perfect-dodge-system)
- [Scanner](#scanner)
- [Boss Fight](#boss-fight)
- [Eight-Directional Sprite System](#eight-directional-sprite-system)
- [Additional Gameplay Footage](#additional-gameplay-footage)

### Level Design

During the level design process, a **blockout** was first created using Unreal Engine 5’s **cube grid system**. 

This level includes two major objectives: 
1. **Find three colored keys** (Blue, Orange, and Red) to unlock different parts of the level.
2. **Activate three generators** to power the passageway to the boss fight.

The blockout allowed me to quickly plan and decide where to place **locked doors** and **generators**, 
helping me experiment with how these elements fit within the level's layout.

<p align="center">
  <img src="https://i.imgur.com/pCFdiDR.png" width="500">
  <br>
</p>

Once the blockout was complete and the core gameplay elements were established, 
the **actual map** was built based on the blockout layout.
The entire level was also fully optimized through **level streaming** to ensure better performance.

<p align="center">
  <img src="https://i.imgur.com/vPEuAoS.png" width="500">
  <br>
  <em>Finished Level</em>
</p>

<p align="center">
  <img src="https://i.imgur.com/oQUBKQK.jpg" width="500">
  <br>
  <em>Boss Arena</em>
</p>

### Perfect Dodge System 
The game features a *Perfect Dodge* system where the player can slow down time if they dodge(Press E) an enemy attack (either melee or ranged) in the last second. 
During a perfect dodge, the environment will shift to black and white, and time will slow around the player, allowing them to take more actions.
While time slows for the enemies and environment, the player themselves remain unaffected by the time slowdown.

<p align="center">
  <img src="https://media.giphy.com/media/HJYw7YHBabJ1qHLwVH/giphy.gif">
  <br>
  <em>Melee Attack Perfect Dodge</em>
</p>

<p align="center">
  <img src="https://media.giphy.com/media/pXhK0TDWxcG7Cb5F5w/giphy.gif">
  <br>
  <em>Enemy Projectile Perfect Dodge</em>
</p>

This *Perfect Dodge* system consists of two major parts:  

1. **Determine if the player's dodge was perfect**  
2. **Slow down time around the player and add the black-and-white effect**  

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

Each time an enemy melee attacks, `addMeleeAttackInfo()` is called. 
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

<p align="center">
  <img src="https://i.imgur.com/CEuTxoO.png" width="500">
</p>

This square box is triggered when it overlaps with the player, which is when the dodge mechanic can be activated and the projectile's attack information is added to the game state. 
Below is the function that handles the overlap and adds the attack information to the game state for enemy projectiles:

```cpp
void AEnemyProjectile::BeginOverlapBoxDodge(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player")) {
		curAttackInfo.StartTime = GetWorld()->GetTimeSeconds();
		curAttackInfo.Duration = attackDuration;
		curAttackInfo.Attacker = this;
		gameStateRef->addAttack(curAttackInfo);
		isAdded = true;
	}
}
```

Once proper `FAttackInfo` structs are stored in the `activeAttacks` array in the game state, the `checkPerfectDodge` function is called every time the player attempts a dodge. 
This function iterates through the `activeAttacks` array and calculates the time until the attack hits. The time until hit (`timeTillHit`) 
is determined by subtracting the current world time from the attack's projected hit time, which is calculated as `StartTime + Duration`.
If the `timeTillHit` is positive and smaller than the player's *perfect dodge window*, the *Perfect Dodge* is performed. 

```cpp
void ADoomCharacter::checkPerfectDodge()
{
	for (FAttackInfo& attackInfo : gameStateRef->getActiveAttacks()) {
		float timeTillHit = attackInfo.StartTime + attackInfo.Duration - GetWorld()->GetTimeSeconds();
		if (timeTillHit > 0 && timeTillHit <= perfectDodgeWindow) {
			perfectDodge();
			break;
		}
	}
}
```

#### Part 2: Perfect Dodge Result and Effect 
Since the time slow effect is only applied to enemies and not the player, simply setting `Global Time Dilation` will not work. 
Instead, when a *Perfect Dodge* is performed, an actor `ABulletTimeAura` is spawned at the player's location. 
This actor functions as a *time bubble*, slowing down all affected actors within its radius while leaving the player unaffected.
  
The `ABulletTimeAura` actor contains a sphere collision component. 
When an enemy or other relevant actor enters this bubble, their `Custom Time Dilation` is modified through `BeginOverlap` function to create the slow-motion effect. 
Once they leave the bubble, their `Custom Time Dilation` is reset to normal through `EndOverlap` function. 

Additionally, `GlobalTimeDilation` is momentarily modified at the very beginning (for 0.05 seconds) to create a *dragging effect*, 
making the transition into slow motion feel more dynamic.   

```cpp
void ADoomCharacter::perfectDodge() 
{
	FVector spawnLocation = GetActorLocation();
	FRotator spawnRotation = GetActorRotation();

	if (myBulletTimeAura) {
		GetWorld()->SpawnActor<ABulletTimeAura>(myBulletTimeAura, spawnLocation, spawnRotation);
	}

	// Create dragging effect in the first 0.05 seconds of the perfect dodge
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);
	GetWorld()->GetTimerManager().SetTimer(perfectDodgeEffectHandle, [&]()
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		}, 0.05, false);
}
```

Finally, to further enhance the *Perfect Dodge* effect, a black-and-white "filter" is applied during the time slow. 
This is achieved by attaching a *Post Process Component* to the `ABulletTimeAura` actor and setting its *Global Saturation* to black.

### Scanner

Pressing `Z` in-game activates the **Environmental Scanner**, which highlights important objects in the surroundings:

- **Enemies**: Highlighted in **Red**
- **Pickups** (Health, Armor, Ammo, Keys): Highlighted in **Green**
- **Weapon Pickups**: Highlighted in **Blue**  

<p align="center">
  <img src="https://media.giphy.com/media/kFlsAvmIrwL93RJ9OZ/giphy.gif">
  <br>
  <em>Scanner</em>
</p>

How it works:  
1. Spwan the [Scanner Actor](Source/Doom/Ability/Scanner.cpp) at the player location, which contains a **Post Process Component** that will apply the scanner material([Material Blueprint](https://blueprintue.com/blueprint/kme0-79b/)).
2. The scanner expanding effect is achieved by updating the material's radius through **Material Parameter Collection** in the scanner's **TimelineUpdate**
3. **Custom Stencils** are assigned to relevant objects, allowing the material to differentiate between enemies, pickups, and weapons.

### Boss Fight

<p align="center">
  <img src="https://media.giphy.com/media/mRZljRnmdSmO3IT1MF/giphy.gif">
  <br>
  <em>Boss Projectile Attack</em>
</p>

<p align="center">
  <img src="gifs/SummonAttack.gif">
  <br>
  <em>Boss Summon Attack</em>
</p>

<p align="center">
  <img src="gifs/DropAttack.gif">
  <br>
  <em>Boss Drop Attack</em>
</p>

<p align="center">
  <img src="gifs/LaserAttack.gif">
  <br>
  <em>Boss Laser Attack</em>
</p>

<p align="center">
  <img src="gifs/RisingOrb.gif">
  <br>
  <em>Boss Rising Orb Attack</em>
</p>


| **Boss Attack**        | **Mechanics**                                                                                                                                      | **Damage Per Hit**                          |
|------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------|
| **Projectile**         | Shoots projectiles in a 120-degree arc                                                                                                             | 6 per projectile                            |
| **Summon**             | Summons 4 self-destructive enemies                                                                                                                 | 25 per enemy                                |
| **Drop**        | Disappears, shows red circle indicator on the floor, then drops to deal AoE damage (radius: 1200, delay: 1.5s)                                    | 20 (AoE)                                    |
| **Laser**       | Continuously shoots laser beams toward the player and summons a laser tower in the center of the arena                                           | Tower: 5/sec, Boss: 6/sec                   |
| **Rising Orb**  | 3 orbs rise into the sky and fly toward the player once at the top. Orbs can be destroyed (each orb has 50 HP)                                   | 10 per orb                                  |





### Eight-Directional Sprite System  
The game features an eight-directional sprite system similar to the classic *Doom (1993)*. 
Enemies dynamically change their sprites based on their position relative to the player, 
creating a sense of being a 3D object while only using sprites.  

<p align="center">
  <img src="https://media2.giphy.com/media/v1.Y2lkPTc5MGI3NjExYmdzNXJpODJ2MHptODd5eTI3ajVvZW9ueGxkdWliM2w5bDVxeG1ueiZlcD12MV9pbnRlcm5naWZfYnlfaWQmY3Q9Zw/gZdBe1mF5ISCfxP2To/giphy.gif">
  <img src="https://media0.giphy.com/media/v1.Y2lkPTc5MGI3NjExNHlocjBud3AybThrZXAzbnB4bXNwZzJ3MXZ6b3p6cjlobjBjODZ4eiZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/s3x1EbznMiVRjNHkeu/giphy.gif">
  <br>
  <em>Eight-Directional Sprite System</em>
</p>

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

`updateFlipbook` is a helper function that updates the flipbook array in use based on an enum called `enemyState`. 
The `relativeDegree` parameter is used to set the relative rotation of the `EnemyFlipBookComponent` with regards to the enemy actor itself, ensuring it always faces the player.  

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

### Additional Gameplay Footage
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemPickup.generated.h"


UENUM(BlueprintType)
enum ItemType {
	Shield UMETA(DisplayName = "Shield"),
	Health UMETA(DisplayName = "Health"),
	BulletAmmo UMETA(DisplayName = "BulletAmmo"),
	ShellAmmo UMETA(DisplayName = "ShellAmmo"),
	RocketAmmo UMETA(DisplayName = "RocketAmmo"),
	CellAmmo UMETA(DisplayName = "CellAmmo"),
	RedKeyCard UMETA(DisplayName = "RedKeyCard"),
	OrangeKeyCard UMETA(DisplayName = "OrangeKeyCard"),
	BlueKeyCard UMETA(DisplayName = "BlueKeyCard"),
};




UCLASS()
class DOOM_API AItemPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ItemType> itemType;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	float itemAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* sphereCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* itemSprite;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	float rotationSpeed;

	void RotateImage(float DeltaTime);

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};

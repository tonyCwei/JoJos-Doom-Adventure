// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WoodCrate.generated.h"

UCLASS()
class DOOM_API AWoodCrate : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWoodCrate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	UGeometryCollectionComponent* woodCrateGeometryCollection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits")
	TArray<TSubclassOf<class AItemPickup>> droppedItemClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* droppedItemPlace;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* boxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	class USoundCue* destroySound;
public:
	UFUNCTION(BlueprintCallable)
	void Destruct(FVector hitLocation);

};

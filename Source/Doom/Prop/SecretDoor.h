// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "SecretDoor.generated.h"

/**
 * 
 */
UCLASS()
class DOOM_API ASecretDoor : public AInteractable
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ASecretDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* movingMesh;


	//Translate
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	bool shouldTranslate = false;

	FVector startingRelativeLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	FVector targetRelativeLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* translateFloatCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* translateTimeline;

	UFUNCTION()
	void translateTimelineUpdate(float Value);
	

	//Rotate
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	bool shouldRotate = false;

	FRotator startingRelativeRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	FRotator targetRelativeRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* rotationFloatCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* rotationTimeline;

	UFUNCTION()
	void rotationTimelineUpdate(float Value);


	//Key Check
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	TArray<FName> keysNeeded;

	bool hasNeededKeys(AActor* interactingActor);


public:

	void interact(FString interactedComponentName, AActor* interactingActor) override;

};

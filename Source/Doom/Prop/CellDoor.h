// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CellDoor.generated.h"


class UStaticMeshComponent;

UCLASS()
class DOOM_API ACellDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACellDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* glassDoorBase;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* glassDoorMove;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class URectLightComponent* cellLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* doorOpenTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* FloatCurve;

	UFUNCTION()
	void doorOpenTimelineUpdate(float Value);

	FVector glassDoorStartLocation;



public:

	UFUNCTION(BlueprintCallable)
	void open();

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Scanner.generated.h"

UCLASS()
class DOOM_API AScanner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AScanner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
public:	



protected:
	FTimerHandle destoryTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Componet", meta = (AllowPrivateAccess = "true"))
	class UPostProcessComponent* PostProcessComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	class USoundCue* scanSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	float hightlightDuration = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	float maxScanRadius = 2500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* scanTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* FloatCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	UMaterialParameterCollection* MaterialParameterCollection;

	UFUNCTION()
	void scanTimelineUpdate(float Value);

	UFUNCTION()
	void scanTimelineFinished();

	

};

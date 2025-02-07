// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BGMManager.generated.h"

UCLASS()
class DOOM_API ABGMManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABGMManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BGM")
	class UAudioComponent* BGMAudioComponent;

	
	USoundBase* CurrentBGM;

	
	

public:


	UFUNCTION(BlueprintCallable)
	void PlayBGM(USoundBase* NewBGM, float FadeInDuration = 1.0f, float Volume = 1.0f);

	UFUNCTION(BlueprintCallable)
	void StopBGM(float FadeOutDuration = 1.0f);

	UFUNCTION(BlueprintCallable)
	void PauseBGM();

	UFUNCTION(BlueprintCallable)
	void ResumeBGM();

	UFUNCTION(BlueprintCallable)
	void setVolume(float newVolume);

	UFUNCTION(BlueprintCallable)
	UAudioComponent* getAudioComponent() { return BGMAudioComponent; };
};

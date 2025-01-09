// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossHUD.generated.h"

/**
 * 
 */
class UTextBlock;
class UImage;
class UBorder;
class UProgressBar;


UCLASS()
class DOOM_API UBossHUD : public UUserWidget
{
	GENERATED_BODY()




protected:

	UPROPERTY(meta = (BindWidget))
	UProgressBar* BossHealthBar;


public:

	UFUNCTION(BlueprintCallable)
	void setBossHealthBarPercent(float curPercent);
};

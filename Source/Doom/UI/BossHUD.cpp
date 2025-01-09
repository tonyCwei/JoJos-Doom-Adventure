// Fill out your copyright notice in the Description page of Project Settings.


#include "BossHUD.h"
#include "Components/ProgressBar.h"

void UBossHUD::setBossHealthBarPercent(float curPercent)
{
	BossHealthBar->SetPercent(curPercent);
}

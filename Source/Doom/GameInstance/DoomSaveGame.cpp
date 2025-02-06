// Fill out your copyright notice in the Description page of Project Settings.


#include "DoomSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"


void UDoomSaveGame::applySettings()
{
	//FullScreen
	if (isFullScreen) {
		UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
		Settings->SetFullscreenMode(EWindowMode::Fullscreen);
		Settings->ApplyResolutionSettings(false);
	}
	else {
		UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
		Settings->SetFullscreenMode(EWindowMode::Windowed);
		Settings->ApplyResolutionSettings(false);
	}
}

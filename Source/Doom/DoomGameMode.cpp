// Copyright Epic Games, Inc. All Rights Reserved.

#include "DoomGameMode.h"
#include "DoomCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADoomGameMode::ADoomGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	/*static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;*/

}

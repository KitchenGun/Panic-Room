// Copyright Epic Games, Inc. All Rights Reserved.

#include "Panic_RoomGameMode.h"
#include "Panic_RoomCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Character/BasicPlayerState.h"

APanic_RoomGameMode::APanic_RoomGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerStateClass = ABasicPlayerState::StaticClass();
}

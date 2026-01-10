// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Panic_RoomGameMode.generated.h"

/**
 *  Simple GameMode for a first person game
 */
UCLASS(abstract)
class APanic_RoomGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APanic_RoomGameMode();
};




// Copyright Epic Games, Inc. All Rights Reserved.

#include "Framework/Panic_RoomGameMode.h"
#include "Panic_RoomCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Character/BasicPlayerState.h"
#include "Attribute/BasicAttributeSet.h"
#include "GameFramework/PlayerStart.h"

APanic_RoomGameMode::APanic_RoomGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerStateClass = ABasicPlayerState::StaticClass();
}

void APanic_RoomGameMode::RequestRespawn(AController* DeadController)
{
	if (!DeadController) return;

	FTimerHandle RespawnTimerHandle;
	FTimerDelegate RespawnDelegate;
	RespawnDelegate.BindUObject(this, &APanic_RoomGameMode::FinishRespawn, DeadController);

	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);
}

void APanic_RoomGameMode::FinishRespawn(AController* DeadController)
{
	if (!DeadController) return;

	// 기존 폰 제거
	if (APawn* OldPawn = DeadController->GetPawn())
	{
		OldPawn->Destroy();
	}

	// PlayerStart에서 새 폰 스폰 및 빙의
	// RestartPlayer 내부에서 FindPlayerStart → SpawnDefaultPawnFor → Possess 순으로 처리됨
	RestartPlayer(DeadController);
}

void APanic_RoomGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	// 리스폰 후 체력을 최대값으로 초기화
	if (ABasicPlayerState* PS = NewPlayer->GetPlayerState<ABasicPlayerState>())
	{
		PS->ResetAttributesForRespawn();

		const UBasicAttributeSet* AttrSet = PS->GetAttributeSet();
		UE_LOG(LogTemp, Warning, TEXT("[Respawn] %s | Health: %.0f / %.0f"),
			*PS->GetPlayerName(),
			AttrSet ? AttrSet->GetHealth() : 0.f,
			AttrSet ? AttrSet->GetMaxHealth() : 0.f);
	}
}

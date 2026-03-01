// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BasicGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyPlayersUpdated);

/**
 * 
 */
UCLASS()
class PANIC_ROOM_API ABasicGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnLobbyPlayersUpdated OnLobbyPlayersUpdated;

protected:
	// 서버 : 플레이어 추가 시 호출
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	// 서버 : 플레이어 제거 시 호출
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

};


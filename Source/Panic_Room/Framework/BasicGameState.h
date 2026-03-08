// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BasicGameState.generated.h"

class ABasicPlayerState;

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

	// 로비 UI에서 바로 바인딩할 수 있도록 현재 접속 플레이어 PlayerState 목록 반환
	UFUNCTION(BlueprintPure, Category = "Lobby")
	TArray<ABasicPlayerState*> GetLobbyPlayerStates() const;

protected:
	// 로컬 : 플레이어 추가 시 호출
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	// 로컬 : 플레이어 제거 시 호출
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	// 클라이언트 PlayerArray 복제 완료 시 호출
	virtual void OnRep_PlayerArray() override;

};

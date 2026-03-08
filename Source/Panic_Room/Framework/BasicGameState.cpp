// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/BasicGameState.h"

void ABasicGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	OnLobbyPlayersUpdated.Broadcast();
}

void ABasicGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	OnLobbyPlayersUpdated.Broadcast();
}



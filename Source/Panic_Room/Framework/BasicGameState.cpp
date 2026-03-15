// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/BasicGameState.h"
#include "Character/BasicPlayerState.h"

TArray<ABasicPlayerState*> ABasicGameState::GetLobbyPlayerStates() const
{
	TArray<ABasicPlayerState*> Out;
	Out.Reserve(PlayerArray.Num());

	for (APlayerState* PS : PlayerArray)
	{
		if (ABasicPlayerState* BasicPS = Cast<ABasicPlayerState>(PS))
		{
			Out.Add(BasicPS);
		}
	}

	return Out;
}

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

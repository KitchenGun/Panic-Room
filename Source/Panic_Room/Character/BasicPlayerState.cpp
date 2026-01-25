// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BasicPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

ABasicPlayerState::ABasicPlayerState()
{
	bReplicates = true;
}

void ABasicPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABasicPlayerState, SteamPlayerName);
}

void ABasicPlayerState::BeginPlay()
{
	Super::BeginPlay();

	// 세션
	if (HasAuthority())
	{
		FetchSteamPlayerName();
	}
}

void ABasicPlayerState::FetchSteamPlayerName()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface();
		if (IdentityInterface.IsValid())
		{
			FString PlayerName = IdentityInterface->GetPlayerNickname(0);
			if (!PlayerName.IsEmpty())
			{
				SteamPlayerName = PlayerName;
				OnRep_SteamPlayerName();
				
				return;
			}
		}
	}

	SteamPlayerName = GetPlayerName();
	OnRep_SteamPlayerName();
}

// 세션 검색 완료 시 성공 여부 알림 함수
void ABasicPlayerState::OnRep_SteamPlayerName()
{
	OnSteamNameChanged.Broadcast(SteamPlayerName);
}

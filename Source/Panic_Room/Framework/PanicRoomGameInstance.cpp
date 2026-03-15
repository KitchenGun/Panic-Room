// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/PanicRoomGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

UPanicRoomGameInstance::UPanicRoomGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, LobbyLevelName(TEXT("LV_Jo"))
	, bHostLobbyRequested(false)
	, bSessionCreated(false)
{
}

void UPanicRoomGameInstance::Init()
{
	Super::Init();

	// 게임 실행 시작 시 상태를 초기화
	bHostLobbyRequested = false;
	bSessionCreated = false;
}

void UPanicRoomGameInstance::RequestHostLobbyAndOpenListen()
{
	bHostLobbyRequested = true;
	bSessionCreated = false;
	OpenLobbyLevelAsListenServer();
}

bool UPanicRoomGameInstance::ConsumeHostLobbyRequested()
{
	const bool bWasRequested = bHostLobbyRequested;
	bHostLobbyRequested = false;
	return bWasRequested;
}

void UPanicRoomGameInstance::OpenLobbyLevelAsListenServer()
{
	if (LobbyLevelName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenLobbyLevelAsListenServer failed: LobbyLevelName is None."));
		return;
	}

	UGameplayStatics::OpenLevel(this, LobbyLevelName, true, TEXT("listen?game=/Game/Blueprints/Framework/GM_Lobby.GM_Lobby_C"));
}

void UPanicRoomGameInstance::TravelToJoinedSession(FName SessionName)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub)
	{
		UE_LOG(LogTemp, Warning, TEXT("TravelToJoinedSession: OnlineSubsystem not found."));
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("TravelToJoinedSession: Session interface not valid."));
		return;
	}

	FString ConnectString;
	if (!Sessions->GetResolvedConnectString(SessionName, ConnectString))
	{
		UE_LOG(LogTemp, Warning, TEXT("TravelToJoinedSession: Failed to get connect string for session '%s'."), *SessionName.ToString());
		return;
	}

	APlayerController* PC = GetFirstLocalPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("TravelToJoinedSession: No local PlayerController found."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("TravelToJoinedSession: Traveling to %s"), *ConnectString);
	PC->ClientTravel(ConnectString, TRAVEL_Absolute);
}

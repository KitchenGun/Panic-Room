// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/PanicRoomGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#if !UE_SERVER
	THIRD_PARTY_INCLUDES_START
	#include "steam/steam_api.h"
	THIRD_PARTY_INCLUDES_END
#endif


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

void UPanicRoomGameInstance::ShowSteamInviteOverlay()
{
#if !UE_SERVER
	if (SteamFriends())
	{
		FNamedOnlineSession* Session = nullptr;
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid())
			{
				Session = Sessions->GetNamedSession(FName(TEXT("GameSession")));
			}
		}

		if (Session && Session->SessionInfo.IsValid())
		{
			FString SessionId = Session->SessionInfo->GetSessionId().ToString();
			uint64 LobbyId = FCString::Strtoui64(*SessionId, nullptr, 10);
			if (LobbyId != 0)
			{
				SteamFriends()->ActivateGameOverlayInviteDialog(CSteamID(LobbyId));
				return;
			}
		}

		// 세션 ID를 못 가져온 경우 일반 친구 목록 표시
		SteamFriends()->ActivateGameOverlay("friends");
	}
#endif
}

void UPanicRoomGameInstance::CreateLobbySession(int32 MaxPlayers)
{
	// Online 서브시스템 체크
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPanicRoomGameInstance::CreateLobbySession - 온라인 서브시스템 없음."));
		OnLobbySessionCreated.Broadcast(false);
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	if (!Sessions.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("UPanicRoomGameInstance::CreateLobbySession - 세션 인터페이스 없음."));
		OnLobbySessionCreated.Broadcast(false);
		return;
	}

	// 기존 세션 존재 시 제거 후 재생성
	FNamedOnlineSession* ExistingSession = Sessions->GetNamedSession(NAME_GameSession);
	if (ExistingSession)
	{
		UE_LOG(LogTemp, Log, TEXT("UPanicRoomGameInstance::CreateLobbySession - 기존 세션 존재. 재생성 실행"));
		DestroySessionDelegateHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(
			FOnDestroySessionCompleteDelegate::CreateLambda([this, MaxPlayers](FName SessionName, bool bWasSuccessful)
			{
				// 제거 및 생성
				IOnlineSubsystem* Sub = IOnlineSubsystem::Get();
				if (Sub)
				{
					Sub->GetSessionInterface()->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionDelegateHandle);
				}
				// 재귀 호출 세션 생성
				CreateLobbySession(MaxPlayers);
			})
		);

		Sessions->DestroySession(NAME_GameSession);
		return;
	}

	// 설정
	FOnlineSessionSettings SessionSettings;
	SessionSettings.NumPublicConnections = MaxPlayers;		// 최대 유저
	SessionSettings.bShouldAdvertise = true;				// 세션 검색 가능 (추후 필요하겠지)
	SessionSettings.bAllowJoinInProgress = true;			// 게임 중 참가 여부
	SessionSettings.bIsLANMatch = false;					// 인터넷 매치
	SessionSettings.bUsesPresence = true;					// Steam Presence사용
	SessionSettings.bAllowJoinViaPresence = true;			// Presence로 참가 가능 여부
	SessionSettings.bUseLobbiesIfAvailable = true;			// Steam 로비 사용
	SessionSettings.bAllowInvites = true;					// 초대 가능

	// 콜백 등록
	CreateSessionDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionCompleteDelegate::CreateUObject(this, &UPanicRoomGameInstance::OnCreateSessionComplete)
		);

	Sessions->CreateSession(0, NAME_GameSession, SessionSettings);
}

void UPanicRoomGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// 콜백 해제
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		OnlineSub->GetSessionInterface()->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionDelegateHandle);
	}

	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("UPanicRoomGameInstance::OnCreateSessionComplete - 세션 '%s' 생성 성공"), *SessionName.ToString());
		bSessionCreated = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPanicRoomGameInstance::OnCreateSessionComplete - 세션 '%s' 생성 실패"), *SessionName.ToString());
	}

	// Blueprint에 결과 전달
	OnLobbySessionCreated.Broadcast(bWasSuccessful);
}

void UPanicRoomGameInstance::DestroyCurrentSessions()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub)
	{
		OnLobbySessionDestroyed.Broadcast(false);
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid())
	{
		OnLobbySessionDestroyed.Broadcast(false);
		return;
	}

	// 세션 파괴 완료 콜백 등록
	DestroySessionDelegateHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(
		FOnDestroySessionCompleteDelegate::CreateUObject(this, &UPanicRoomGameInstance::OnDestroySessionComplete)
	);

	UE_LOG(LogTemp, Log, TEXT("UPanicRoomGameInstance::OnDestroySessionComplete - 세션 파괴 요청"));
	Sessions->DestroySession(NAME_GameSession);
}

void UPanicRoomGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	// 콜백 해제
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		OnlineSub->GetSessionInterface()->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionDelegateHandle);
	}

	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("UPanicRoomGameInstance::OnDestroySessionComplete - 세션 파괴 성공"));
		bSessionCreated = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPanicRoomGameInstance::OnDestroySessionComplete - 세션 파괴 실패"));
	}

	OnLobbySessionDestroyed.Broadcast(bWasSuccessful);
}
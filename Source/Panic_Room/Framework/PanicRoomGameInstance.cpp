// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/PanicRoomGameInstance.h"

#include "Kismet/GameplayStatics.h"

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

	UGameplayStatics::OpenLevel(this, LobbyLevelName, true, TEXT("listen"));
}

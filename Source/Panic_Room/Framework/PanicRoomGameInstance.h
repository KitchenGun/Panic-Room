// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "PanicRoomGameInstance.generated.h"

/**
 * [26-03-01][JJH]
 * Lobby Host State를 관리하는 게임인스턴스
 *
 */
UCLASS()
class PANIC_ROOM_API UPanicRoomGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()

public:
	UPanicRoomGameInstance(const FObjectInitializer& ObjectInitializer);

	// 호스트 로비 요청 및 리슨 서버로 레벨 오픈
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void RequestHostLobbyAndOpenListen();

	// 호스트 요청 플래그를 소비 (true 반환 후 즉시 false로 초기화)
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	bool ConsumeHostLobbyRequested();

	UFUNCTION(BlueprintPure, Category = "Lobby")
	bool IsHostLobbyRequested() const { return bHostLobbyRequested; }

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SetSessionCreated(bool bCreated) { bSessionCreated = bCreated; }

	UFUNCTION(BlueprintPure, Category = "Lobby")
	bool HasCreatedSession() const { return bSessionCreated; }

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void OpenLobbyLevelAsListenServer();

protected:
	virtual void Init() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby")
	FName LobbyLevelName;

private:
	UPROPERTY(Transient)
	bool bHostLobbyRequested;

	UPROPERTY(Transient)
	bool bSessionCreated;
};

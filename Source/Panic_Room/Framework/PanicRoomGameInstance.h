// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "PanicRoomGameInstance.generated.h"

/**
 * [26-03-01][JJH]
 * Lobby Host State를 래핑하는 프로젝트 Instance
 * 
 */
UCLASS()
class PANIC_ROOM_API UPanicRoomGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()

public:
	UPanicRoomGameInstance(const FObjectInitializer& ObjectInitializer);

	// 호스트 흐름을 표시 및 로비 맵을 리슨 서버로 염.
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void RequestHostLobbyAndOpenListen();

	// 호스트-플로우 전환이 보류 중일 때 한 번 true를 반환하고, 그 후 플래그를 해제합니다.
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

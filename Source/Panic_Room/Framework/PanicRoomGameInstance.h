// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "PanicRoomGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbySessionResult, bool, bWasSuccessful);

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

	// 세선 참가 후 서버 레벨로 클라이언트 이동
	UFUNCTION(BlueprintCallable, Category = "Session")
	void TravelToJoinedSession(FName SessionName = TEXT("GameSession"));

	// Steam 친구 초대 오버레이 열기
	UFUNCTION(BlueprintCallable, Category = "Steam")
	void ShowSteamInviteOverlay();

	// 로비 세션 생성 (Host 전용, Level 이동 후 호출)
	// [JJH] TODO : 우선 4명으로 설정했는데 추후 바꾸든지 하자
	UFUNCTION(BlueprintCallable, Category = "Session")
	void CreateLobbySession(int32 MaxPlayers = 4);

	// 현재 세션 제거
	UFUNCTION(BlueprintCallable, Category = "Session")
	void DestroyCurrentSessions();

	// 세션 생성 결과 (Blueprint에서 바인딩 및 UI 체크 가능)
	UPROPERTY(BlueprintAssignable, Category = "Session")
	FOnLobbySessionResult OnLobbySessionCreated;
	// 세션 제거 결과
	UPROPERTY(BlueprintAssignable, Category = "Session")
	FOnLobbySessionResult OnLobbySessionDestroyed;

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

	// 세션 생성 완료 콜백
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	// 세션 제거 완료 콜백
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	FDelegateHandle CreateSessionDelegateHandle;
	FDelegateHandle DestroySessionDelegateHandle;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "BasicPlayerState.generated.h"

class UDA_StartUpDataBase;
class UTexture2D;

// Steam 이름 변경 시 UI 컴포넌트 업데이트 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSteamNameChanged, const FString&, NewName);
// Steam 아바타 로드 완료 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSteamAvatarLoaded, UTexture2D*, AvatarTexture);

UCLASS()
class PANIC_ROOM_API ABasicPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	ABasicPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetGADefault(UAbilitySystemComponent* ASC);

	//Get ASC Attribute
	UFUNCTION(BlueprintCallable, Category = "GAS")
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
	virtual void BeginPlay() override;

	void FetchSteamPlayerName();

	UPROPERTY(ReplicatedUsing = OnRep_SteamPlayerName, BlueprintReadOnly, Category = "Player")
	FString SteamPlayerName;

	UFUNCTION()
	void OnRep_SteamPlayerName();

public:
	// 현재 Steam 이름 반환
	UFUNCTION(BlueprintCallable, Category = "Player")
	FString GetSteamPlayerName() const { return SteamPlayerName; }

	// 아바타 텍스처 요청 (캐싱됨, 여러 번 호출해도 안전)
	UFUNCTION(BlueprintCallable, Category = "Player")
	void RequestSteamAvatar();

	// 이름 변경 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Player")
	FOnSteamNameChanged OnSteamNameChanged;

	// 아바타 로드 완료 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Player")
	FOnSteamAvatarLoaded OnSteamAvatarLoaded;

	// 로드된 아바타 텍스처 (로컬 캐시, 복제 안 됨)
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<UTexture2D> CachedAvatarTexture;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<class UAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSoftObjectPtr<UDA_StartUpDataBase> StartUpDataBase;

	// 아바타 재시도 타이머
	FTimerHandle AvatarRetryTimerHandle;
};

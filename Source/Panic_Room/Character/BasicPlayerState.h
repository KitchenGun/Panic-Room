// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "BasicPlayerState.generated.h"


 // Steam 이름 변경 시 UI 업데이트 델리게이트 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSteamNameChanged, const FString&, NewName);

UCLASS()
class PANIC_ROOM_API ABasicPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:

	ABasicPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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
	// 유저 이름 변경 업데이트용 이벤트
	UFUNCTION(BlueprintCallable, Category = "Player")
	FString GetSteamPlayerName() const { return SteamPlayerName; }

	// 현재 유저 이름 저장변수
	UPROPERTY(BlueprintAssignable, Category = "Player")
	FOnSteamNameChanged OnSteamNameChanged;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<class UAttributeSet> AttributeSet;
};

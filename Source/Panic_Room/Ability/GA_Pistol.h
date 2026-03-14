// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/GA_Basic.h"
#include "GA_Pistol.generated.h"


class APanic_RoomCharacter;
class ABasicPlayerState;
class ABasicWeapon;

UCLASS()
class PANIC_ROOM_API UGA_Pistol : public UGA_Basic
{
	GENERATED_BODY()

public:
	UGA_Pistol();

	UFUNCTION(BlueprintPure, Category = "Ability")
	APanic_RoomCharacter* GetPanicRoomCharacterFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability")
	ABasicPlayerState* GetBasicPlayerStateFromActorInfo() const;

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	// 에디터에서 IA_Fire InputAction 지정
	UPROPERTY(EditDefaultsOnly, Category = "Ability|Fire")
	class UInputAction* FireAction;

private:
	mutable TWeakObjectPtr<APanic_RoomCharacter> CachedPanicRoomCharacter;
	mutable TWeakObjectPtr<ABasicPlayerState> CachedBasicPlayerState;
	TWeakObjectPtr<ABasicWeapon> CachedWeapon;

	// EnhancedInput 바인딩 핸들 (해제용)
	int32 FireInputHandle = -1;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/GA_Basic.h"
#include "GA_Pistol.generated.h"


class APanic_RoomCharacter;
class ABasicPlayerState;
class UPanic_RoomWeaponComponent;

UCLASS()
class PANIC_ROOM_API UGA_Pistol : public UGA_Basic
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Ability")
	APanic_RoomCharacter* GetPanicRoomCharacterFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability")
	ABasicPlayerState* GetBasicPlayerStateFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability")
	UPanic_RoomWeaponComponent* GetPanicRoomWeaponComponentFromActorInfo() const;

private:
	mutable TWeakObjectPtr<APanic_RoomCharacter> CachedPanicRoomCharacter;
	mutable	TWeakObjectPtr<ABasicPlayerState> CachedBasicPlayerState;
};

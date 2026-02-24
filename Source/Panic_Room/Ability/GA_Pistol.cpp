// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Panic_RoomCharacter.h"
#include "Character/BasicPlayerState.h"
#include "ActorComponent/Panic_RoomWeaponComponent.h"
#include "Ability/GA_Pistol.h"

APanic_RoomCharacter* UGA_Pistol::GetPanicRoomCharacterFromActorInfo() const
{
	if (!CachedPanicRoomCharacter.IsValid())
	{
		CachedPanicRoomCharacter = Cast<APanic_RoomCharacter>(CurrentActorInfo->AvatarActor);
	}
	return CachedPanicRoomCharacter.Get();
}

ABasicPlayerState* UGA_Pistol::GetBasicPlayerStateFromActorInfo() const
{
	if(!CachedBasicPlayerState.IsValid())
	{
		CachedBasicPlayerState = Cast<ABasicPlayerState>(CurrentActorInfo->PlayerController->PlayerState);
	}
	return CachedBasicPlayerState.IsValid() ? CachedBasicPlayerState.Get():nullptr;
}

UPanic_RoomWeaponComponent* UGA_Pistol::GetPanicRoomWeaponComponentFromActorInfo() const
{
	
	return nullptr;//GetPanicRoomCharacterFromActorInfo()->GetPanicRoomWeaponComponent();
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"


namespace PanicRoomGameplayTags
{
	PANIC_ROOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_Pistol_Equip);
	PANIC_ROOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_Pistol_UnEquip);
	PANIC_ROOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_Pistol_Fire);
	PANIC_ROOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	PANIC_ROOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look);
	PANIC_ROOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Jump);
	PANIC_ROOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Fire);
}
/**
 * 
 */
class PANIC_ROOM_API GameplayTag
{
public:
	GameplayTag();
	~GameplayTag();
};

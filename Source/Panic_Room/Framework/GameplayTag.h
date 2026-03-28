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

	// 체력/데미지 이벤트 태그
	PANIC_ROOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Character_Death);

	// GameplayEffect 식별 태그
	PANIC_ROOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Damage);
	PANIC_ROOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Heal);

	// 상태 태그
	PANIC_ROOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Dead);
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

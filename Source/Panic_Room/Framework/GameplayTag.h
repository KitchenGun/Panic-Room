// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"


namespace PanicRoomGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_Pistol_Fire);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Jump);

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

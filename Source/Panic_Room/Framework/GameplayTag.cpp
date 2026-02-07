// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/GameplayTag.h"


namespace PanicRoomGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Pistol, "Weapon.Pistol.Fire");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "Input.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Jump, "Input.Jump");
}


GameplayTag::GameplayTag()
{
}

GameplayTag::~GameplayTag()
{
}

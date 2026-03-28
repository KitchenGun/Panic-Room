// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/GameplayTag.h"


namespace PanicRoomGameplayTags
{
    UE_DEFINE_GAMEPLAY_TAG(Weapon_Pistol_Equip, "Weapon.Pistol.Equip");
    UE_DEFINE_GAMEPLAY_TAG(Weapon_Pistol_UnEquip, "Weapon.Pistol.UnEquip");
    UE_DEFINE_GAMEPLAY_TAG(Weapon_Pistol_Fire, "Weapon.Pistol.Fire");
    UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
    UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look");
    UE_DEFINE_GAMEPLAY_TAG(InputTag_Jump, "InputTag.Jump");
    UE_DEFINE_GAMEPLAY_TAG(InputTag_Fire, "InputTag.Fire");

    UE_DEFINE_GAMEPLAY_TAG(Event_Character_Death,  "Event.Character.Death");
    UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_Damage,  "GameplayEffect.Damage");
    UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_Heal,    "GameplayEffect.Heal");
    UE_DEFINE_GAMEPLAY_TAG(Status_Dead,            "Status.Dead");
}


GameplayTag::GameplayTag()
{
}

GameplayTag::~GameplayTag()
{
}

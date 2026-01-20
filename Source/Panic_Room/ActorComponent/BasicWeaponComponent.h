// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponent/Panic_RoomWeaponComponent.h"
#include "BasicWeaponComponent.generated.h"

/**
 * 
 */
UCLASS()
class PANIC_ROOM_API UBasicWeaponComponent : public UPanic_RoomWeaponComponent
{
	GENERATED_BODY()
public:
	virtual void Fire() override;
};

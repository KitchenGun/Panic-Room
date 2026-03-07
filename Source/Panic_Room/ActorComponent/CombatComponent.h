// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponent/PawnExtensionComponent.h"
#include "GameplayTagContainer.h"
#include "CombatComponent.generated.h"


class UPanic_RoomPickUpComponent;

UCLASS()
class PANIC_ROOM_API UCombatComponent : public UPawnExtensionComponent
{
	GENERATED_BODY()

public:

	//UFUNCTION(BlueprintCallable, Category = "Combat")
	//void RegisterSpawnedWeapon(FGameplayTag WeaponTag,UPanic_RoomPickUpComponent* WeaponPickUpComponent,bool bIsEquipped = false);

	//UFUNCTION(BlueprintCallable,Category = "Combat")



private:

	//Pair<FGameplayTag, UPanic_RoomPickUpComponent*> ;
};

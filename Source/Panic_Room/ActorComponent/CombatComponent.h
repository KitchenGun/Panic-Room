// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponent/PawnExtensionComponent.h"
#include "GameplayTagContainer.h"
#include "CombatComponent.generated.h"


class ABasicWeapon;

UCLASS()
class PANIC_ROOM_API UCombatComponent : public UPawnExtensionComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, ABasicWeapon* InWeaponToRegister, bool bRegisterAsEquippedWeapon = false);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	ABasicWeapon* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	ABasicWeapon* GetCharacterCurrentEquippedWeapon() const;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	FGameplayTag CurrentEquippedWeaponTag;

private:

	TMap<FGameplayTag, ABasicWeapon*> CharacterCarriedWeaponMap;
};

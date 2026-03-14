// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponent/PawnExtensionComponent.h"
#include "GameplayTagContainer.h"
#include "CombatComponent.generated.h"


class ABasicWeapon;

/**
 * UCombatComponent
 *
 * 캐릭터의 무기 소지 및 장착 상태를 관리하는 컴포넌트.
 * GameplayTag를 키로 사용하여 무기를 등록하고 조회한다.
 */
UCLASS()
class PANIC_ROOM_API UCombatComponent : public UPawnExtensionComponent
{
	GENERATED_BODY()

public:

	/**
	 * 스폰된 무기를 컴포넌트에 등록한다.
	 *
	 * @param InWeaponTagToRegister  무기를 식별할 GameplayTag (중복 등록 시 assert)
	 * @param InWeaponToRegister     등록할 무기 액터 (null이면 assert)
	 * @param bRegisterAsEquippedWeapon  true이면 등록과 동시에 현재 장착 무기로 설정
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, ABasicWeapon* InWeaponToRegister, bool bRegisterAsEquippedWeapon = false);

	/**
	 * 태그에 해당하는 소지 무기를 반환한다.
	 *
	 * @param InWeaponTagToGet  조회할 무기의 GameplayTag
	 * @return 태그에 매핑된 무기 액터. 없으면 nullptr
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	ABasicWeapon* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;

	/**
	 * 현재 장착 중인 무기를 반환한다.
	 * CurrentEquippedWeaponTag가 유효하지 않으면 nullptr을 반환한다.
	 *
	 * @return 현재 장착된 무기 액터. 없으면 nullptr
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	ABasicWeapon* GetCharacterCurrentEquippedWeapon() const;

	/** 현재 장착된 무기의 GameplayTag. 블루프린트에서 직접 읽기/쓰기 가능 */
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	FGameplayTag CurrentEquippedWeaponTag;

private:

	/** GameplayTag를 키로, 소지 중인 무기 액터를 값으로 저장하는 맵 */
	TMap<FGameplayTag, ABasicWeapon*> CharacterCarriedWeaponMap;
};

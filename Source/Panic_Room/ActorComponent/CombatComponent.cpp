// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/CombatComponent.h"
#include "Weapon/BasicWeapon.h"

void UCombatComponent::RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, ABasicWeapon* InWeaponToRegister, bool bRegisterAsEquippedWeapon)
{
	// 동일한 태그로 이미 등록된 무기가 있으면 중복 등록 방지
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister),
		TEXT("A weapon named %s has already been added as a carried weapon"),
		*InWeaponTagToRegister.ToString());

	// 유효하지 않은 무기 포인터 방지
	check(InWeaponToRegister);

	// 태그와 무기를 맵에 추가
	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister, InWeaponToRegister);

	// 장착 무기로 등록 요청 시 현재 장착 태그를 갱신
	if (bRegisterAsEquippedWeapon)
	{
		CurrentEquippedWeaponTag = InWeaponTagToRegister;
	}

	// 등록 완료 로그 출력
	UE_LOG(LogTemp, Log, TEXT("A weapon named: %s has been registered using the tag %s"),
		*InWeaponToRegister->GetName(), *InWeaponTagToRegister.ToString());
}

ABasicWeapon* UCombatComponent::GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const
{
	// 맵에 해당 태그가 존재하는지 먼저 확인
	if (CharacterCarriedWeaponMap.Contains(InWeaponTagToGet))
	{
		// Find는 포인터의 포인터를 반환하므로 역참조하여 반환
		if (ABasicWeapon* const* FoundWeapon = CharacterCarriedWeaponMap.Find(InWeaponTagToGet))
		{
			return *FoundWeapon;
		}
	}
	return nullptr;
}

ABasicWeapon* UCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
	// 장착 태그가 유효하지 않으면 장착된 무기 없음
	if (!CurrentEquippedWeaponTag.IsValid())
	{
		return nullptr;
	}

	// 현재 장착 태그로 맵에서 무기를 조회하여 반환
	return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
}

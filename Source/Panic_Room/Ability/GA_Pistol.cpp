// Fill out your copyright notice in the Description page of Project Settings.

#include "Ability/GA_Pistol.h"
#include "ActorComponent/CombatComponent.h"
#include "Weapon/BasicWeapon.h"

UGA_Pistol::UGA_Pistol()
{
	// 발사 후 즉시 종료 → 인스턴스 유지 불필요
	// GAS 쿨다운·비용은 CDO 기준으로 적용되므로 NonInstanced로 충분
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

void UGA_Pistol::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// CombatComponent에서 현재 장착 무기 조회
	// GA_Grap_SpawnWeapon이 사전에 무기를 등록해 두어야 한다
	UCombatComponent* CombatComp = GetCombatComponentFromActorInfo();
	if (!CombatComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GA_Pistol] CombatComponent를 찾을 수 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ABasicWeapon* Weapon = CombatComp->GetCharacterCurrentEquippedWeapon();
	if (!Weapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GA_Pistol] 장착된 무기 없음 - GA_Grap_SpawnWeapon 선행 확인 필요"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 발사 실행
	Weapon->Fire();

	// GAS 흐름 안에서 종료 → 쿨다운·비용이 이 시점에 정산됨
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

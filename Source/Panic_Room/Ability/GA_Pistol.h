// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/GA_Basic.h"
#include "GA_Pistol.generated.h"

/**
 * UGA_Pistol
 *
 * 권총 발사 전용 어빌리티.
 *
 * - ActivationPolicy : OnInputTriggered  (InputTag_Fire 입력 시 활성화)
 * - InstancingPolicy : NonInstanced      (발사 후 즉시 종료, 인스턴스 불필요)
 *
 * 무기 스폰·장착 관리는 GA_Grap_SpawnWeapon(Blueprint)이 담당한다.
 * 이 어빌리티는 CombatComponent에서 현재 장착 무기를 가져와
 * Fire()를 호출한 뒤 즉시 종료한다.
 *
 * GAS 쿨다운·탄약 비용·태그 차단은 이 어빌리티에 설정한다.
 */
UCLASS()
class PANIC_ROOM_API UGA_Pistol : public UGA_Basic
{
	GENERATED_BODY()

public:
	UGA_Pistol();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
};

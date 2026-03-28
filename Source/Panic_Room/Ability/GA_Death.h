// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/GA_Basic.h"
#include "GA_Death.generated.h"

/**
 * UGA_Death
 *
 * 사망 처리 전용 어빌리티.
 *
 * - TriggerSource : GameplayEvent (Event.Character.Death)
 * - 생성자에서 AbilityTriggers에 등록 → ASC::HandleGameplayEvent 호출 시 자동 활성화
 * - NetExecutionPolicy : ServerOnly → 서버에서만 실행
 * - DA_StartUpDataBase의 ReactiveAbilities에 등록해야 동작한다.
 */
UCLASS()
class PANIC_ROOM_API UGA_Death : public UGA_Basic
{
	GENERATED_BODY()

public:
	UGA_Death();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
};

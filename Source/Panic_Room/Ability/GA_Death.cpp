// Fill out your copyright notice in the Description page of Project Settings.

#include "Ability/GA_Death.h"
#include "Character/Panic_RoomCharacter.h"
#include "Framework/GameplayTag.h"
#include "Abilities/GameplayAbilityTypes.h"

UGA_Death::UGA_Death()
{
	// 이벤트로만 활성화되므로 인스턴스 불필요
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;

	// 서버에서만 실행 - 리스폰은 서버 권한 필요
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	// Event.Character.Death GameplayEvent 수신 시 자동 활성화
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag    = PanicRoomGameplayTags::Event_Character_Death;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_Death::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (APanic_RoomCharacter* Character = Cast<APanic_RoomCharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->HandleDeath();
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

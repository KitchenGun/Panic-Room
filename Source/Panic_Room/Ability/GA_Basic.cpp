// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_Basic.h"
#include "AbilitySystemComponent.h"
#include "Character/Panic_RoomCharacter.h"
#include "ActorComponent/CombatComponent.h"

UCombatComponent* UGA_Basic::GetCombatComponentFromActorInfo() const
{
	return GetAvatarActorFromActorInfo()->FindComponentByClass<UCombatComponent>();
}

UAbilitySystemComponent* UGA_Basic::GetASCFromActorInfo() const
{
	return GetCurrentActorInfo()->AbilitySystemComponent.Get();
}

void UGA_Basic::SendGameplayEventToASC(FGameplayTag EventTag) const
{
	UAbilitySystemComponent* ASC = GetASCFromActorInfo();
	if (!ASC) return;

	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	ASC->HandleGameplayEvent(EventTag, &Payload);
}

void UGA_Basic::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	if (ActivationPolicy == EAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

void UGA_Basic::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Basic.generated.h"

/**
 * 
 */
UCLASS()
class PANIC_ROOM_API UGA_Basic : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;
	//https://dong-grae.tistory.com/225?category=1510793
	//UPROPERTY(EditDefaultsOnly, Category = "Ability")
	//EAbilityActivationPolicy ActivationPolicy;
};

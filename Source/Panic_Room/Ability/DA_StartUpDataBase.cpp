// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/DA_StartUpDataBase.h"
#include "AbilitySystemComponent.h"
#include "Ability/GA_Basic.h"


void UDA_StartUpDataBase::GiveToAbilitySystemComponent(UAbilitySystemComponent* ASC, int32 ApplyLevel)
{
	check(ASC);

	SetAbilities(ActivateOnGivenAbilities, ASC, ApplyLevel);
	SetAbilities(ReactiveAbilites, ASC, ApplyLevel);

}


void UDA_StartUpDataBase::SetAbilities(TArray<TSubclassOf<UGA_Basic>> Abilities, UAbilitySystemComponent* ASC, int32 ApplyLevel)
{
	if(Abilities.IsEmpty())
		return;

	for (TSubclassOf<UGA_Basic>& Ability : Abilities)
	{
		if(!Ability)
			continue;

		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject = ASC->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;

		// GA_Basic에 설정된 InputTag를 DynamicAbilityTags에 등록 (입력 → 어빌리티 매핑)
		const UGA_Basic* AbilityCDO = Ability->GetDefaultObject<UGA_Basic>();
		if (AbilityCDO && AbilityCDO->InputTag.IsValid())
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AbilityCDO->InputTag);
		}

		ASC->GiveAbility(AbilitySpec);
		UE_LOG(LogTemp, Display, TEXT("Given ability: %s"), *Ability->GetName());
	}

}

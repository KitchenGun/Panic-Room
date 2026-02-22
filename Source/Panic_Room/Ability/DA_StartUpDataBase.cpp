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

		ASC->GiveAbility(AbilitySpec);
	}

}

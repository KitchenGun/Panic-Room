// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/BasicAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "Framework/GameplayTag.h"
#include "AbilitySystemComponent.h"

UBasicAttributeSet::UBasicAttributeSet()
{
	// Health 초기값 설정
	InitHealth(100.f);
	InitMaxHealth(100.f);
	InitDamage(0.f);
}

void UBasicAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Health,    COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	// Damage는 Meta Attribute — 복제 안 함
}

void UBasicAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// MaxHealth 변경 시 0 이하 방지
	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}

	// Health 변경 시 [0, MaxHealth] 범위 클램핑
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}

void UBasicAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Damage Meta Attribute가 적용된 경우 Health에 반영
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float DamageValue = GetDamage();

		if (DamageValue > 0.f)
		{
			const float NewHealth = FMath::Clamp(GetHealth() - DamageValue, 0.f, GetMaxHealth());
			SetHealth(NewHealth);

			// Damage Meta Attribute 초기화
			SetDamage(0.f);

			const FString VictimName = GetOwningActor() ? GetOwningActor()->GetName() : TEXT("Unknown");
			const FString InstigatorName = Data.EffectSpec.GetContext().GetInstigator()
				? Data.EffectSpec.GetContext().GetInstigator()->GetName() : TEXT("Unknown");
			UE_LOG(LogTemp, Warning, TEXT("[Hit] %s ← %s | Damage: %.0f | Health: %.0f / %.0f"),
				*VictimName, *InstigatorName, DamageValue, NewHealth, GetMaxHealth());

			// 사망 판정: Health가 0이 되면 GA_Death를 트리거하는 이벤트 전송
			// GA_Death(ReactiveAbility)가 이 이벤트를 수신하여 HandleDeath를 호출한다
			if (NewHealth <= 0.f)
			{
				if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
				{
					FGameplayEventData EventData;
					EventData.Instigator = Data.EffectSpec.GetContext().GetInstigator();
					EventData.Target     = GetOwningActor();

					ASC->HandleGameplayEvent(PanicRoomGameplayTags::Event_Character_Death, &EventData);
				}
			}
		}
	}
}

void UBasicAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Health, OldHealth);
}

void UBasicAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, MaxHealth, OldMaxHealth);
}

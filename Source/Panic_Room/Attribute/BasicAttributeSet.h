// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BasicAttributeSet.generated.h"

// GAS 표준 Attribute 접근자 매크로
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class PANIC_ROOM_API UBasicAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UBasicAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Attribute 변경 후 처리 (Damage Meta Attribute → Health 반영)
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// Attribute 변경 전 처리 (범위 클램핑)
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	// ─────────────────────────────────────────────────────
	// Health
	// ─────────────────────────────────────────────────────

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attribute|Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBasicAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attribute|Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBasicAttributeSet, MaxHealth)

	// ─────────────────────────────────────────────────────
	// Meta Attribute: 데미지 (복제 안 함, GE 실행 시 임시 값)
	// ─────────────────────────────────────────────────────

	UPROPERTY(BlueprintReadOnly, Category = "Attribute|Meta")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UBasicAttributeSet, Damage)

protected:

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
};

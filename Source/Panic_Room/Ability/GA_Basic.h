// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GA_Basic.generated.h"

class UCombatComponent;
class UAbilitySystemComponent;


UENUM(BlueprintType)
enum class EAbilityActivationPolicy : uint8
{
	OnInputTriggered UMETA(DisplayName = "On Input Triggered"),
	WhileInputActive UMETA(DisplayName = "While Input Active"),
	OnSpawn UMETA(DisplayName = "On Spawn"),
	OnGiven UMETA(DisplayName = "On Given")
};

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
	UFUNCTION(BlueprintPure, Category = "PanicRoom|Ability")
	UCombatComponent* GetCombatComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "PanicRoom|Ability")
	UAbilitySystemComponent* GetASCFromActorInfo() const;

	/** ASC가 PlayerState에 있는 경우에도 올바르게 GameplayEvent를 전송 */
	UFUNCTION(BlueprintCallable, Category = "PanicRoom|Ability")
	void SendGameplayEventToASC(FGameplayTag EventTag) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	EAbilityActivationPolicy ActivationPolicy;

public:
	/** 이 어빌리티를 발동할 입력 태그. DA_StartUpDataBase가 부여 시 DynamicAbilityTags에 자동 등록 */
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	FGameplayTag InputTag;
};

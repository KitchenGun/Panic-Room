// Fill out your copyright notice in the Description page of Project Settings.

#include "Ability/GA_Pistol.h"
#include "Character/Panic_RoomCharacter.h"
#include "Character/BasicPlayerState.h"
#include "ActorComponent/CombatComponent.h"
#include "Weapon/BasicWeapon.h"
#include "EnhancedInputComponent.h"

UGA_Pistol::UGA_Pistol()
{
	// 장착 중 지속 활성 → InstancedPerActor 필수
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

APanic_RoomCharacter* UGA_Pistol::GetPanicRoomCharacterFromActorInfo() const
{
	if (!CachedPanicRoomCharacter.IsValid())
	{
		CachedPanicRoomCharacter = Cast<APanic_RoomCharacter>(CurrentActorInfo->AvatarActor);
	}
	return CachedPanicRoomCharacter.Get();
}

ABasicPlayerState* UGA_Pistol::GetBasicPlayerStateFromActorInfo() const
{
	if (!CachedBasicPlayerState.IsValid())
	{
		CachedBasicPlayerState = Cast<ABasicPlayerState>(CurrentActorInfo->PlayerController->PlayerState);
	}
	return CachedBasicPlayerState.IsValid() ? CachedBasicPlayerState.Get() : nullptr;
}

void UGA_Pistol::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APanic_RoomCharacter* Character = GetPanicRoomCharacterFromActorInfo();
	UCombatComponent* CombatComp = GetCombatComponentFromActorInfo();

	if (!Character || !CombatComp || !FireAction)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GA_Pistol] 활성화 실패 - Character/CombatComp/FireAction 확인"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ABasicWeapon* Weapon = CombatComp->GetCharacterCurrentEquippedWeapon();
	if (!Weapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GA_Pistol] 장착된 무기 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CachedWeapon = Weapon;

	// GAS를 거치지 않고 직접 Fire() 바인딩 → 빠른 반복 발사 가능
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(Character->InputComponent))
	{
		FireInputHandle = EIC->BindAction(FireAction, ETriggerEvent::Started, Weapon, &ABasicWeapon::Fire).GetHandle();
		UE_LOG(LogTemp, Log, TEXT("[GA_Pistol] 발사 바인딩 완료"));
	}
	// EndAbility 호출하지 않음 → 해제 전까지 지속 활성
}

void UGA_Pistol::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// 발사 바인딩 제거
	APanic_RoomCharacter* Character = GetPanicRoomCharacterFromActorInfo();
	if (Character && FireInputHandle != -1)
	{
		if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(Character->InputComponent))
		{
			EIC->RemoveBindingByHandle(FireInputHandle);
			FireInputHandle = -1;
			UE_LOG(LogTemp, Log, TEXT("[GA_Pistol] 발사 바인딩 제거"));
		}
	}

	CachedWeapon.Reset();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

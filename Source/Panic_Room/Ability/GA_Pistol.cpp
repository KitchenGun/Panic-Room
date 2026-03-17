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
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ABasicWeapon* Weapon = CombatComp->GetCharacterCurrentEquippedWeapon();
	if (!Weapon)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CachedWeapon = Weapon;

	// InputComponent가 준비되지 않았으면 다음 틱에 재시도
	if (!TryBindFireInput())
	{
		RetryBindFireInput();
	}
	// EndAbility 호출하지 않음 → 해제 전까지 지속 활성
}

bool UGA_Pistol::TryBindFireInput()
{
	APanic_RoomCharacter* Character = GetPanicRoomCharacterFromActorInfo();
	ABasicWeapon* Weapon = CachedWeapon.Get();

	if (!Character || !Weapon || !FireAction)
	{
		return false;
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(Character->InputComponent);
	if (!EIC)
	{
		return false;
	}

	// GAS를 거치지 않고 직접 Fire() 바인딩 → 빠른 반복 발사 가능
	FireInputHandle = EIC->BindAction(FireAction, ETriggerEvent::Started, Weapon, &ABasicWeapon::Fire).GetHandle();
	return true;
}

void UGA_Pistol::RetryBindFireInput()
{
	if (TryBindFireInput())
	{
		return;
	}

	// 아직 준비 안 됨 → 다음 틱에 재시도
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateUObject(this, &UGA_Pistol::RetryBindFireInput));
	}
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
		}
	}

	CachedWeapon.Reset();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

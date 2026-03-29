// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/CombatComponent.h"
#include "Weapon/BasicWeapon.h"
#include "Character/Panic_RoomCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Framework/GameplayTag.h"

UCombatComponent::UCombatComponent()
{
	// 복제 활성화 (EquippedWeapon 복제를 위해 필요)
	SetIsReplicatedByDefault(true);
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
}

// ── 클라이언트 복제 수신 ──────────────────────────────────────
void UCombatComponent::OnRep_EquippedWeapon()
{
	if (!EquippedWeapon) return;

	// 클라이언트 측 태그 설정
	if (!CurrentEquippedWeaponTag.IsValid())
	{
		CurrentEquippedWeaponTag = PanicRoomGameplayTags::Weapon_Pistol_Equip;
	}

	// 클라이언트 측 맵에도 등록 (태그 기반 조회 호환)
	if (!CharacterCarriedWeaponMap.Contains(CurrentEquippedWeaponTag))
	{
		CharacterCarriedWeaponMap.Emplace(CurrentEquippedWeaponTag, EquippedWeapon);
	}

	// 클라이언트에서도 캐릭터에 부착 (아직 안 된 경우)
	APanic_RoomCharacter* Character = Cast<APanic_RoomCharacter>(GetPawnOwner());
	if (Character && !EquippedWeapon->GetOwnerCharacter())
	{
		EquippedWeapon->AttachToCharacter(Character);
	}

	UE_LOG(LogTemp, Display, TEXT("[CombatComponent] OnRep_EquippedWeapon: %s (Client)"),
		*EquippedWeapon->GetName());
}

// ── 무기 등록 (GA_Grap_SpawnWeapon 블루프린트에서 호출) ───────
void UCombatComponent::RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, ABasicWeapon* InWeaponToRegister, bool bRegisterAsEquippedWeapon)
{
	// 동일한 태그로 이미 등록된 무기가 있으면 중복 등록 방지
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister),
		TEXT("A weapon named %s has already been added as a carried weapon"),
		*InWeaponTagToRegister.ToString());

	// 유효하지 않은 무기 포인터 방지
	check(InWeaponToRegister);

	// 태그와 무기를 맵에 추가
	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister, InWeaponToRegister);

	// 장착 무기로 등록 요청 시 현재 장착 태그 + 복제 포인터 갱신
	if (bRegisterAsEquippedWeapon)
	{
		CurrentEquippedWeaponTag = InWeaponTagToRegister;
		EquippedWeapon = InWeaponToRegister;
	}

	// 등록 완료 로그 출력
	UE_LOG(LogTemp, Log, TEXT("A weapon named: %s has been registered using the tag %s"),
		*InWeaponToRegister->GetName(), *InWeaponTagToRegister.ToString());
}

ABasicWeapon* UCombatComponent::GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const
{
	// 맵에 해당 태그가 존재하는지 먼저 확인
	if (CharacterCarriedWeaponMap.Contains(InWeaponTagToGet))
	{
		// Find는 포인터의 포인터를 반환하므로 역참조하여 반환
		if (ABasicWeapon* const* FoundWeapon = CharacterCarriedWeaponMap.Find(InWeaponTagToGet))
		{
			return *FoundWeapon;
		}
	}
	return nullptr;
}

ABasicWeapon* UCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
	// 1차: 복제된 포인터 직접 반환 (서버·클라이언트 모두 신뢰성 높음)
	if (EquippedWeapon)
	{
		return EquippedWeapon;
	}

	// 2차: 태그 기반 맵 조회 (폴백)
	if (CurrentEquippedWeaponTag.IsValid())
	{
		return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
	}

	return nullptr;
}

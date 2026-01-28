// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BasicPlayerState.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

ABasicPlayerState::ABasicPlayerState()
{

	// ASC 생성
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	// 멀티플레이어 최적화: PlayerState에 있을 경우 소유 클라이언트에만 상세 이펙트 전달, 타인에겐 최소화
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// AttributeSet 생성
	AttributeSet = CreateDefaultSubobject<UAttributeSet>(TEXT("AttributeSet"));

	// 복제 활성화
	bReplicates = true;

	// 네트워크 갱신 빈도 설정 (멀티플레이어 최적화)
	NetUpdateFrequency = 100.0f;
}

void ABasicPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABasicPlayerState, SteamPlayerName);
}

void ABasicPlayerState::BeginPlay()
{
	Super::BeginPlay();

	// 세션
	if (HasAuthority())
	{
		FetchSteamPlayerName();
	}
}

void ABasicPlayerState::FetchSteamPlayerName()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface();
		if (IdentityInterface.IsValid())
		{
			FString PlayerName = IdentityInterface->GetPlayerNickname(0);
			if (!PlayerName.IsEmpty())
			{
				SteamPlayerName = PlayerName;
				OnRep_SteamPlayerName();

				return;
			}
		}
	}

	SteamPlayerName = GetPlayerName();
	OnRep_SteamPlayerName();
}

// 세션 검색 완료 시 성공 여부 알림 함수
void ABasicPlayerState::OnRep_SteamPlayerName()
{
	OnSteamNameChanged.Broadcast(SteamPlayerName);
}


UAbilitySystemComponent* ABasicPlayerState::GetAbilitySystemComponent() const
{
    return ASC;
}

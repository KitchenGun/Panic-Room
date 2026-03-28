// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BasicPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Attribute/BasicAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Ability/DA_StartUpDataBase.h"
#include "Engine/Texture2D.h"

#if !UE_SERVER
THIRD_PARTY_INCLUDES_START
#include "steam/steam_api.h"
THIRD_PARTY_INCLUDES_END
#endif

ABasicPlayerState::ABasicPlayerState()
{
	// ASC 생성
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	// 멀티플레이어 최적화: PlayerState에 ASC를 두어 리스폰 후에도 유지, Mixed 모드로 클라이언트 예측 허용
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// AttributeSet 생성
	AttributeSet = CreateDefaultSubobject<UBasicAttributeSet>(TEXT("AttributeSet"));

	// 복제 활성화
	bReplicates = true;

	// 네트워크 갱신 빈도 (멀티플레이어 최적화)
	NetUpdateFrequency = 100.0f;
}


void ABasicPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABasicPlayerState, SteamPlayerName);
}

void ABasicPlayerState::SetGADefault(UAbilitySystemComponent* InASC)
{
	// 이미 어빌리티가 부여된 경우 중복 방지 (리스폰 시 PossessedBy 재호출 대비)
	if (InASC->GetActivatableAbilities().Num() > 0)
	{
		UE_LOG(LogTemp, Display, TEXT("[SetGADefault] Abilities already granted — skipping"));
		return;
	}

	if (UDA_StartUpDataBase* LoadedData = StartUpDataBase.LoadSynchronous())
	{
		LoadedData->GiveToAbilitySystemComponent(InASC);
	}
}

void ABasicPlayerState::ResetAttributesForRespawn()
{
	if (AttributeSet)
	{
		// Health를 MaxHealth로 복구
		AttributeSet->InitHealth(AttributeSet->GetMaxHealth());
	}
}

void ABasicPlayerState::BeginPlay()
{
	Super::BeginPlay();

	// 서버(호스트)에서만 Steam 이름 조회 후 복제
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
			// UniqueNetId로 해당 플레이어의 Steam 닉네임 조회
			TSharedPtr<const FUniqueNetId> PlayerNetId = GetUniqueId().GetUniqueNetId();
			if (PlayerNetId.IsValid())
			{
				FString PlayerName = IdentityInterface->GetPlayerNickname(*PlayerNetId);
				if (!PlayerName.IsEmpty())
				{
					SteamPlayerName = PlayerName;
					OnRep_SteamPlayerName();

					return;
				}
			}
		}
	}

	// Steam 이름을 가져오지 못한 경우 엔진 기본 이름 사용
	SteamPlayerName = GetPlayerName();
	OnRep_SteamPlayerName();
}

// 이름 복제 완료 시 실행되는 콜백 (서버에서는 수동 호출)
void ABasicPlayerState::OnRep_SteamPlayerName()
{
	OnSteamNameChanged.Broadcast(SteamPlayerName);
	RequestSteamAvatar();
}

void ABasicPlayerState::RequestSteamAvatar()
{
	// 이미 캐시된 경우 바로 브로드캐스트
	if (CachedAvatarTexture)
	{
		OnSteamAvatarLoaded.Broadcast(CachedAvatarTexture);
		return;
	}

#if !UE_SERVER
	TSharedPtr<const FUniqueNetId> NetId = GetUniqueId().GetUniqueNetId();
	if (!NetId.IsValid()) return;

	// Steam NetId는 SteamID64 숫자 문자열로 표현됨
	uint64 SteamID64 = FCString::Strtoui64(*NetId->ToString(), nullptr, 10);
	if (SteamID64 == 0) return;

	if (!SteamFriends() || !SteamUtils()) return;

	CSteamID SteamUser(SteamID64);

	// Large avatar (184x184) 요청; -1이면 Steam이 아직 로딩 중
	int32 AvatarHandle = SteamFriends()->GetLargeFriendAvatar(SteamUser);
	if (AvatarHandle == -1)
	{
		// Steam에 유저 정보 요청 후 1초 뒤 재시도
		SteamFriends()->RequestUserInformation(SteamUser, false);
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				AvatarRetryTimerHandle, this,
				&ABasicPlayerState::RequestSteamAvatar,
				1.0f, false
			);
		}
		return;
	}
	if (AvatarHandle <= 0) return;

	uint32 Width = 0, Height = 0;
	if (!SteamUtils()->GetImageSize(AvatarHandle, &Width, &Height) || Width == 0) return;

	TArray<uint8> RGBAData;
	RGBAData.SetNumUninitialized(Width * Height * 4);
	if (!SteamUtils()->GetImageRGBA(AvatarHandle, RGBAData.GetData(), RGBAData.Num())) return;

	UTexture2D* NewTexture = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);
	if (!NewTexture) return;

	FTexture2DMipMap& Mip = NewTexture->GetPlatformData()->Mips[0];
	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(Data, RGBAData.GetData(), RGBAData.Num());
	Mip.BulkData.Unlock();
	NewTexture->UpdateResource();

	CachedAvatarTexture = NewTexture;
	OnSteamAvatarLoaded.Broadcast(CachedAvatarTexture);
#endif
}

UAbilitySystemComponent* ABasicPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BasicPlayerState.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"

ABasicPlayerState::ABasicPlayerState()
{
    // ASC 생성
    ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

    // 멀티플레이어 최적화: PlayerState에 있을 경우 소유 클라이언트에만 상세 이펙트 전달, 타인에겐 최소화
    ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    // AttributeSet 생성
    AttributeSet = CreateDefaultSubobject<UAttributeSet>(TEXT("AttributeSet"));

    // 네트워크 갱신 빈도 설정 (멀티플레이어 최적화)
    NetUpdateFrequency = 100.0f;
}

UAbilitySystemComponent* ABasicPlayerState::GetAbilitySystemComponent() const
{
    return ASC;
}

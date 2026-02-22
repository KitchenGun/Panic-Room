// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_StartUpDataBase.generated.h"


class UGA_Basic;
class UAbilitySystemComponent;

UCLASS()
class PANIC_ROOM_API UDA_StartUpDataBase : public UDataAsset
{
	GENERATED_BODY()
	
public:
	//character asc에 초기능력 부여
	virtual void GiveToAbilitySystemComponent(UAbilitySystemComponent* ASC, int32 ApplyLevel = 1);

protected:
	//캐릭터 생성되자마자 즉시 부여하고 활성화할 능력들
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StartupAbilities")
	TArray<TSubclassOf<UGA_Basic>> ActivateOnGivenAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StartupAbilities")
	TArray<TSubclassOf<UGA_Basic>> ReactiveAbilites;

	void SetAbilities(TArray<TSubclassOf<UGA_Basic>> Abilities, UAbilitySystemComponent* ASC, int32 ApplyLevel=1);

};

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Panic_RoomGameMode.generated.h"

UCLASS(minimalapi)
class APanic_RoomGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APanic_RoomGameMode();

	/**
	 * 사망한 컨트롤러의 리스폰을 요청한다.
	 * RespawnDelay 초 후에 PlayerStart에서 새 폰을 스폰한다.
	 * BasicAttributeSet → Character::HandleDeath → 이 함수 순으로 호출된다.
	 */
	void RequestRespawn(AController* DeadController);

	/** 리스폰까지 대기 시간 (초). 에디터 또는 자식 BP에서 조정 가능 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Respawn")
	float RespawnDelay = 3.f;

protected:
	/** RestartPlayer 이후 체력을 최대값으로 초기화 */
	virtual void RestartPlayer(AController* NewPlayer) override;

private:
	/** 타이머 만료 후 실제 리스폰 처리 */
	UFUNCTION()
	void FinishRespawn(AController* DeadController);
};




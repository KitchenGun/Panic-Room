// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasicWeapon.generated.h"

class UBoxComponent;
class APanic_RoomCharacter;

UCLASS()
class PANIC_ROOM_API ABasicWeapon : public AActor
{
	GENERATED_BODY()

public:
	ABasicWeapon();

	FORCEINLINE UBoxComponent* GetWeaponCollision() const { return WeaponCollision; }
	FORCEINLINE APanic_RoomCharacter* GetOwnerCharacter() const { return OwnerCharacter.Get(); }

	/** 캐릭터 메시의 지정 소켓에 무기를 부착 */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void AttachToCharacter(APanic_RoomCharacter* InOwnerCharacter);

	/** 캐릭터에서 무기 분리 */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void DetachFromCharacter();

	/** 발사 - 라인트레이스 기반 */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire();

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	UBoxComponent* WeaponCollision;

	/** 캐릭터 메시에서 무기를 붙일 소켓 이름 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Attachment")
	FName WeaponSocketName = TEXT("GripPoint");

	/** 총구 소켓 이름 (라인트레이스 시작점) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Fire")
	FName MuzzleSocketName = TEXT("Muzzle");

	/** 발사 사운드 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Fire")
	USoundBase* FireSound;

	/** 발사 시 캐릭터 팔 애니메이션 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Fire")
	UAnimMontage* FireAnimation;

	/** 소유 캐릭터 약참조 */
	UPROPERTY()
	TWeakObjectPtr<APanic_RoomCharacter> OwnerCharacter;
};

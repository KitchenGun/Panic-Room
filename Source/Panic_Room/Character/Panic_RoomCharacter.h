// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemInterface.h"
#include "Panic_RoomCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UDA_InputConfig;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class APanic_RoomCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()


	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* CombatComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	
	
public:
	APanic_RoomCharacter();

	virtual void BeginPlay() override;

	// IAbilitySystemInterface — AvatarActor(Character) 기준으로 ASC를 조회할 수 있도록 구현.
	// GAS 내부 유틸, GameplayCue, TargetActor 등이 Character를 통해 ASC를 찾을 때 사용된다.
	// 실제 ASC는 PlayerState가 소유하며 이 함수는 그것을 그대로 반환한다.
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;


protected:

	#pragma region Input
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	#pragma endregion Input


	// APawn interface
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	//빙의되었을 때 호출되는 함수 (서버 전용)
	virtual void PossessedBy(AController* NewController) override;

	// PlayerState 복제 완료 시 호출 (클라이언트 — ASC 초기화 2차 경로)
	virtual void OnRep_PlayerState() override;

	// Controller 복제 완료 시 호출 (클라이언트 — ASC 초기화 3차 백업 경로)
	virtual void OnRep_Controller() override;

	/**
	 * 클라이언트 ASC 초기화 공통 헬퍼.
	 * PossessedBy / OnRep_PlayerState / OnRep_Controller / Input 폴백에서 호출.
	 * PlayerState가 유효해지는 최초 시점에 1회 실행되고, 이후 호출은 무시된다.
	 */
	void InitializeASC();

	void Input_AbilityInputPressed(FGameplayTag InInputTag);
	void Input_AbilityInputReleased(FGameplayTag InInputTag);

	UFUNCTION()
	void OnRep_IsDead();

public:

	#pragma region Death
	/**
	 * 체력이 0이 되었을 때 호출.
	 * GA_Death(ReactiveAbility)가 Event.Character.Death 수신 시 이 함수를 호출한다.
	 * 내부에서 HasAuthority() 검사 — 서버에서만 실행된다.
	 */
	void HandleDeath();

	/** 사망 상태 여부 (중복 호출 방지, 클라이언트 복제) */
	UPROPERTY(ReplicatedUsing = OnRep_IsDead, BlueprintReadOnly, Category = "State")
	bool bIsDead = false;
	#pragma endregion Death


#pragma region Component
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	/** Returns CombatComponent subobject **/
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
#pragma endregion Component

protected:
	#pragma region Input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UDA_InputConfig* InputConfigDataAsset;
	#pragma endregion Input

	TObjectPtr<class ABasicPlayerState> PlayerState;
};


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameplayTag.h"
#include "DA_InputConfig.h"
#include "PanicRoomInputComponent.generated.h"

UCLASS()
class PANIC_ROOM_API UPanicRoomInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<typename UserObject, typename CallbackFunc>
	void BindNativeInputAction
	(
		UDA_InputConfig* InputConfig,//
		const FGameplayTag& InputTag,//���� action ã�� �±�
		ETriggerEvent TriggerEvent,//Ʈ���� �̺�Ʈ Ÿ��
		UserObject* ContextObject,//���ε��� ������Ʈ
		CallbackFunc Func //���ε��� ���� �Լ�
	);
	template<typename UserObject, typename CallbackFunc>
	void BindAbilityInputAction
	(
		UDA_InputConfig* InputConfig,	//action ���Ǵ�� �����;��
		UserObject* ContextObject,		//���ε��� ������Ʈ
		CallbackFunc InputPressFunc,	//Press ���ε��� ���� �Լ�
		CallbackFunc InputReleaseFunc	//Release ���ε��� ���� �Լ�
	);


};

template<typename UserObject, typename CallbackFunc>
inline void UPanicRoomInputComponent::BindNativeInputAction
(
	UDA_InputConfig* InputConfig,	//action ���Ǵ�� �����;��
	const FGameplayTag& InputTag,	//���� action ã�� �±�
	ETriggerEvent TriggerEvent,		//Ʈ���� �̺�Ʈ Ÿ��
	UserObject* ContextObject,		//���ε��� ������Ʈ
	CallbackFunc Func				//���ε��� ���� �Լ�
)
{
	checkf(InputConfig, TEXT("InputConfig is nullptr"));

	if (UInputAction* FoundAction = InputConfig->FindNativeInputActionByTag(InputTag))
	{
		BindAction(FoundAction, TriggerEvent, ContextObject, Func);
	}
}

template<typename UserObject, typename CallbackFunc>
inline void UPanicRoomInputComponent::BindAbilityInputAction
(
	UDA_InputConfig* InputConfig,	//action ���Ǵ��	�����;��
	UserObject* ContextObject,		//���ε��� ������Ʈ
	CallbackFunc InputPressFunc,	//Press ���ε��� ���� �Լ�
	CallbackFunc InputReleaseFunc	//Release ���ε��� ���� �Լ�
)
{
	checkf(InputConfig, TEXT("InputConfig is nullptr"));
	for (const FInputActionConfig& AbilityInputActionConfig : InputConfig->AbilityInputActions)
	{
		if (AbilityInputActionConfig.IsValid())
		{
			BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Started, ContextObject, InputPressFunc,AbilityInputActionConfig.InputTag);
			BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Completed, ContextObject, InputReleaseFunc, AbilityInputActionConfig.InputTag);
		}
	}
}
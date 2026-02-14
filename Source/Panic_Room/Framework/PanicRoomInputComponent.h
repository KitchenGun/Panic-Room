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
		const FGameplayTag& InputTag,//바인 action 찾을 태그
		ETriggerEvent TriggerEvent,//트리거 이벤트 타입
		UserObject* ContextObject,//바인딩할 오브젝트
		CallbackFunc Func //바인딩할 동작 함수
	);

};

template<typename UserObject, typename CallbackFunc>
inline void UPanicRoomInputComponent::BindNativeInputAction
(
	UDA_InputConfig* InputConfig,	//action 정의담긴	데이터어셋
	const FGameplayTag& InputTag,	//바인 action 찾을 태그
	ETriggerEvent TriggerEvent,		//트리거 이벤트 타입
	UserObject* ContextObject,		//바인딩할 오브젝트
	CallbackFunc Func				//바인딩할 동작 함수
)
{
	checkf(InputConfig, TEXT("InputConfig is nullptr"));

	if (UInputAction* FoundAction = InputConfig->FindNativeInputActionByTag(InputTag))
	{
		this->BindAction(FoundAction, TriggerEvent, ContextObject, Func);
	}
}

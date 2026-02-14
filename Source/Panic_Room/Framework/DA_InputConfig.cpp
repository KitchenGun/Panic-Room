// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/DA_InputConfig.h"
#include "GameplayTags.h"

UInputAction* UDA_InputConfig::FindNativeInputActionByTag(const FGameplayTag& InputTag)
{
	for (const FInputActionConfig& ActionConfig : NativeInputActions)
	{
		if (ActionConfig.InputTag == InputTag)
		{
			return ActionConfig.InputAction;
		}
	}
	return nullptr;
}

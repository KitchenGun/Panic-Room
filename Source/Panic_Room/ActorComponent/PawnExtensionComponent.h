// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnExtensionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PANIC_ROOM_API UPawnExtensionComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	template<class T>
	T* GetPawnOwner() const
	{
		static_assert(TPointerIsConvertibleFromTo<T,APawn>::Value,"T must be a type derived from APawn");
		return CastChecked<T>(GetOwner());
	}


	APawn* GetPawnOwner() const
	{
		return CastChecked<APawn>(GetOwner());
	}


	template<class T>
	T* GetController() const
	{
		static_assert(TPointerIsConvertibleFromTo<T,AController>::Value,"T must be a type derived from AController");
		return CastChecked<T>(GetPawnOwner()->GetController());
	}
	
};

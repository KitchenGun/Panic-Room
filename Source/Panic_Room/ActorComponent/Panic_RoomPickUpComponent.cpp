// Copyright Epic Games, Inc. All Rights Reserved.

#include "Panic_RoomPickUpComponent.h"

UPanic_RoomPickUpComponent::UPanic_RoomPickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void UPanic_RoomPickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UPanic_RoomPickUpComponent::OnSphereBeginOverlap);
}

void UPanic_RoomPickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	APanic_RoomCharacter* Character = Cast<APanic_RoomCharacter>(OtherActor);
	if(Character != nullptr)
	{
		// Notify that the actor is being picked up
		OnPickUp.Broadcast(Character);

		// Unregister from the Overlap Event so it is no longer triggered
		OnComponentBeginOverlap.RemoveAll(this);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/BasicWeaponComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Panic_RoomProjectile.h"
#include "../Character/Panic_RoomCharacter.h"	

void UBasicWeaponComponent::Fire()
{
	Super::Fire();
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

		//충돌 파라미터
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner());

		// Line trace (raycast) to check for hits
		bool bHit = World->LineTraceSingleByChannel
		(
			HitResult,
			SpawnLocation,
			SpawnLocation + (PlayerController->PlayerCameraManager->GetActorForwardVector() * 10000.0f),
			ECC_Visibility,
			QueryParams
		);

		DrawDebugLine(GetWorld(),
			SpawnLocation,
			SpawnLocation + (PlayerController->PlayerCameraManager->GetActorForwardVector() * 10000.0f), FColor::Red, false, 2.0f);

		if (bHit)
		{
			AActor* HitActor = HitResult.GetActor();
			UE_LOG(LogTemp, Display, TEXT("Hit Object: %s"), *HitActor->GetName());
		}
	}

	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}

	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

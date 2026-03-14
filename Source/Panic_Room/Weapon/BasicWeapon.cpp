// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/BasicWeapon.h"
#include "Components/BoxComponent.h"
#include "Character/Panic_RoomCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

ABasicWeapon::ABasicWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	WeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollision"));
	WeaponCollision->SetupAttachment(RootComponent);
	WeaponCollision->SetBoxExtent(FVector(20.0f));
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABasicWeapon::AttachToCharacter(APanic_RoomCharacter* InOwnerCharacter)
{
	if (!InOwnerCharacter) return;

	OwnerCharacter = InOwnerCharacter;
	SetOwner(InOwnerCharacter);

	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(InOwnerCharacter->GetMesh1P(), AttachRules, WeaponSocketName);
}

void ABasicWeapon::DetachFromCharacter()
{
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	DetachFromActor(DetachRules);

	OwnerCharacter.Reset();
	SetOwner(nullptr);
}

void ABasicWeapon::Fire()
{
	APanic_RoomCharacter* Character = OwnerCharacter.Get();
	if (!Character || !Character->GetController()) return;

	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	if (!PlayerController) return;

	const FVector MuzzleLocation = (WeaponMesh && WeaponMesh->DoesSocketExist(MuzzleSocketName))
		? WeaponMesh->GetSocketLocation(MuzzleSocketName)
		: GetActorLocation();

	const FRotator CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	const FVector TraceEnd = MuzzleLocation + CameraRotation.Vector() * 10000.0f;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(Character);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		MuzzleLocation,
		TraceEnd,
		ECC_Visibility,
		QueryParams
	);

	DrawDebugLine(GetWorld(), MuzzleLocation, bHit ? HitResult.ImpactPoint : TraceEnd, FColor::Red, false, 2.0f);

	if (bHit && HitResult.GetActor())
	{
		UE_LOG(LogTemp, Display, TEXT("[Fire] Hit: %s"), *HitResult.GetActor()->GetName());
	}

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}

	if (FireAnimation)
	{
		if (UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

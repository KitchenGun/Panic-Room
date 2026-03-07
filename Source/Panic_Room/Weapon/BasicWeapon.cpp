// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/BasicWeapon.h"
#include "Components/BoxComponent.h"

// Sets default values
ABasicWeapon::ABasicWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	WeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollision"));
	WeaponCollision->SetupAttachment(RootComponent);
	WeaponCollision->SetBoxExtent(FVector(20.0f));
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}



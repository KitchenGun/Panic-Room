// Copyright Epic Games, Inc. All Rights Reserved.

#include "Panic_RoomCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PanicRoomInputComponent.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "DA_InputConfig.h"
#include "GameplayTags.h"
#include "BasicPlayerState.h"
//GAS
#include "AbilitySystemComponent.h"



DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// APanic_RoomCharacter

APanic_RoomCharacter::APanic_RoomCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

}

void APanic_RoomCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	PlayerState = Cast<ABasicPlayerState>(GetPlayerState());
}

//////////////////////////////////////////////////////////////////////////// Input

void APanic_RoomCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void APanic_RoomCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	ULocalPlayer* LocalPlayer = Cast<APlayerController>(GetController())->GetLocalPlayer();
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->AddMappingContext(DefaultMappingContext, 0);

	UPanicRoomInputComponent* InputComp = Cast<UPanicRoomInputComponent>(PlayerInputComponent);
	
	InputComp->BindNativeInputAction(InputConfigDataAsset, PanicRoomGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &APanic_RoomCharacter::Look);
	InputComp->BindNativeInputAction(InputConfigDataAsset, PanicRoomGameplayTags::InputTag_Jump, ETriggerEvent::Started, this, &APanic_RoomCharacter::Jump);
	InputComp->BindNativeInputAction(InputConfigDataAsset, PanicRoomGameplayTags::InputTag_Jump, ETriggerEvent::Completed, this, &APanic_RoomCharacter::StopJumping);
	InputComp->BindNativeInputAction(InputConfigDataAsset, PanicRoomGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &APanic_RoomCharacter::Move);
}

void APanic_RoomCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PlayerState = Cast<ABasicPlayerState>(GetPlayerState());

	if (PlayerState)
	{
		PlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(PlayerState, this);
		PlayerState->SetGADefault(PlayerState->GetAbilitySystemComponent());
	}
	
}


void APanic_RoomCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void APanic_RoomCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
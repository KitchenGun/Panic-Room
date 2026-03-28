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
#include "CombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Framework/Panic_RoomGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Attribute/BasicAttributeSet.h"
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

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

}

void APanic_RoomCharacter::BeginPlay()
{
	Super::BeginPlay();
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

	InputComp->BindAbilityInputAction(InputConfigDataAsset, this, &ThisClass::Input_AbilityInputPressed, &ThisClass::Input_AbilityInputReleased);
}

void APanic_RoomCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 서버: PlayerState 캐싱 + ASC 초기화 + 어빌리티 부여
	PlayerState = Cast<ABasicPlayerState>(GetPlayerState());
	if (PlayerState)
	{
		PlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(PlayerState, this);
		PlayerState->SetGADefault(PlayerState->GetAbilitySystemComponent());
	}
}

void APanic_RoomCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// 클라이언트: PlayerState 복제 완료 시 ASC 초기화
	// 어빌리티 부여(SetGADefault)는 서버의 PossessedBy에서 이미 처리됨 — 복제로 전달
	PlayerState = Cast<ABasicPlayerState>(GetPlayerState());
	if (PlayerState)
	{
		PlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(PlayerState, this);
	}
}


void APanic_RoomCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	UAbilitySystemComponent* ASC = PlayerState ? PlayerState->GetAbilitySystemComponent() : nullptr;
	if (!ASC || !InInputTag.IsValid()) return;

	bool bFound = false;
	for (const FGameplayAbilitySpec& AbilitySpec : ASC->GetActivatableAbilities())
	{
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag)) continue;
		bFound = true;
		ASC->TryActivateAbility(AbilitySpec.Handle);
	}

	if (!bFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Input] Tag [%s] 에 매핑된 어빌리티 없음 (DynamicAbilityTags 확인 필요)"), *InInputTag.ToString());
	}
}

void APanic_RoomCharacter::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
	UAbilitySystemComponent* ASC = PlayerState ? PlayerState->GetAbilitySystemComponent() : nullptr;
	if (!ASC || !InInputTag.IsValid()) return;

	for (const FGameplayAbilitySpec& AbilitySpec : ASC->GetActivatableAbilities())
	{
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag)) continue;
		ASC->AbilitySpecInputReleased(const_cast<FGameplayAbilitySpec&>(AbilitySpec));
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

void APanic_RoomCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APanic_RoomCharacter, bIsDead);
}

void APanic_RoomCharacter::HandleDeath()
{
	// 서버에서만 실행 / 중복 방지
	if (!HasAuthority() || bIsDead) return;

	bIsDead = true;
	OnRep_IsDead(); // 서버에서 직접 적용

	// GameMode 에 리스폰 요청
	if (APanic_RoomGameMode* GM = GetWorld()->GetAuthGameMode<APanic_RoomGameMode>())
	{
		GM->RequestRespawn(GetController());
	}
}

void APanic_RoomCharacter::OnRep_IsDead()
{
	if (!bIsDead) return;

	// 이동 및 콜리전 비활성화 (서버·클라이언트 공통)
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	// 소유 클라이언트의 입력 차단
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}
}

// ── 디버그 전용 ────────────────────────────────────────────────
void APanic_RoomCharacter::DebugDamage(float Amount)
{
	if (!HasAuthority()) return;

	ABasicPlayerState* PS = Cast<ABasicPlayerState>(GetPlayerState());
	if (!PS) return;

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC) return;

	UBasicAttributeSet* AttrSet = const_cast<UBasicAttributeSet*>(
		Cast<UBasicAttributeSet>(PS->GetAttributeSet()));
	if (!AttrSet) return;

	// GAS 경로를 통한 데미지 적용 (Instant GE 동적 생성)
	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddInstigator(this, this);

	// UGameplayEffect CDO를 동적으로 생성하여 Damage Meta Attribute에 값 적용
	UGameplayEffect* DamageEffect = NewObject<UGameplayEffect>(GetTransientPackage(), TEXT("DebugDamageEffect"));
	DamageEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

	int32 Idx = DamageEffect->Modifiers.Num();
	DamageEffect->Modifiers.SetNum(Idx + 1);
	FGameplayModifierInfo& ModInfo = DamageEffect->Modifiers[Idx];
	ModInfo.Attribute = UBasicAttributeSet::GetDamageAttribute();
	ModInfo.ModifierOp = EGameplayModOp::Additive;
	ModInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(Amount));

	ASC->ApplyGameplayEffectToSelf(DamageEffect, 1.f, Context);

	UE_LOG(LogTemp, Warning, TEXT("[Debug] %.0f 데미지 적용 → Health: %.0f / %.0f"),
		Amount, AttrSet->GetHealth(), AttrSet->GetMaxHealth());
}

void APanic_RoomCharacter::DebugHealth()
{
	ABasicPlayerState* PS = Cast<ABasicPlayerState>(GetPlayerState());
	if (!PS) return;

	const UBasicAttributeSet* AttrSet = Cast<UBasicAttributeSet>(PS->GetAttributeSet());
	if (!AttrSet) return;

	UE_LOG(LogTemp, Warning, TEXT("[Debug] Health: %.0f / %.0f | Dead: %s"),
		AttrSet->GetHealth(), AttrSet->GetMaxHealth(),
		bIsDead ? TEXT("Yes") : TEXT("No"));
}
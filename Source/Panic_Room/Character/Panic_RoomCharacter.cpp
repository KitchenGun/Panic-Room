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

UAbilitySystemComponent* APanic_RoomCharacter::GetAbilitySystemComponent() const
{
	// ASC는 PlayerState가 소유. 캐시(PlayerState 멤버)가 유효하면 바로 반환하고,
	// 아직 초기화 전이라면 GetPlayerState()를 통해 직접 조회한다.
	if (PlayerState)
	{
		return PlayerState->GetAbilitySystemComponent();
	}

	if (ABasicPlayerState* PS = GetPlayerState<ABasicPlayerState>())
	{
		return PS->GetAbilitySystemComponent();
	}

	return nullptr;
}

void APanic_RoomCharacter::InitializeASC()
{
	ABasicPlayerState* PS = Cast<ABasicPlayerState>(GetPlayerState());
	if (!PS) return;

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC) return;

	// AbilityActorInfo가 이미 이 캐릭터(AvatarActor)로 초기화되어 있으면 스킵.
	// PlayerState 캐시 유무 대신 실제 초기화 여부를 확인하므로,
	// 리스폰 후 새 캐릭터 인스턴스가 들어왔을 때도 정확하게 재초기화된다.
	if (ASC->AbilityActorInfo.IsValid() && ASC->AbilityActorInfo->AvatarActor == this)
	{
		PlayerState = PS; // 캐시만 최신화
		return;
	}

	PlayerState = PS;

	// ASC에 OwnerActor(PlayerState)와 AvatarActor(이 캐릭터)를 알려줌
	ASC->InitAbilityActorInfo(PS, this);

	UE_LOG(LogTemp, Display, TEXT("[InitializeASC] %s | Server: %s"),
		*GetName(), HasAuthority() ? TEXT("Y") : TEXT("N"));
}

// ── 경로 1: 서버 전용 ────────────────────────────────────────
void APanic_RoomCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 서버: ASC 초기화
	InitializeASC();

	if (PlayerState)
	{
		UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();

		// 리스폰 시: 기존 어빌리티를 모두 제거하고 새 폰(AvatarActor)으로 재부여.
		// SourceObject 갱신만 하면 어빌리티 상태가 오염될 수 있으므로
		// ClearAllAbilities → 플래그 리셋 → SetGADefault 재호출 순서로 처리.
		if (PlayerState->WasAbilitiesGranted())
		{
			ASC->ClearAllAbilities();
			PlayerState->ResetAbilitiesGranted();
			CombatComponent->ClearCarriedWeapons();
		}

		// 최초 스폰 및 리스폰 양쪽 모두 SetGADefault 호출로 능력 부여
		PlayerState->SetGADefault(ASC);
	}

}

// ── 경로 2: 클라이언트 — PlayerState 복제 수신 ──────────────
void APanic_RoomCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitializeASC();
}

// ── 경로 3: 클라이언트 — Controller 복제 수신 (백업) ────────
void APanic_RoomCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();
	InitializeASC();
}


void APanic_RoomCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	// 경로 4: 위 경로들이 모두 실패한 경우 최후 Lazy Init
	if (!PlayerState)
	{
		InitializeASC();
	}

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

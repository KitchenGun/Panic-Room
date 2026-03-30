// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/BasicWeapon.h"
#include "Components/BoxComponent.h"
#include "Character/Panic_RoomCharacter.h"
#include "Character/BasicPlayerState.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

ABasicWeapon::ABasicWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	// 멀티플레이: 서버에서 스폰 → 클라이언트로 복제
	bReplicates = true;

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

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(Character);

	// ── Multi Line Trace: 관통된 모든 액터 수집 ─────────────────
	TArray<FHitResult> HitResults;
	const bool bHit = GetWorld()->LineTraceMultiByChannel(
		HitResults,
		MuzzleLocation,
		TraceEnd,
		ECC_Visibility,
		QueryParams
	);

	// ── 디버그: 트레이스 라인 ──────────────────────────────────
	// 히트 없으면 TraceEnd까지, 히트 있으면 가장 먼 히트 지점까지 빨간선
	// ASC 대상에게 데미지가 적용된 경우 해당 지점에 초록 구체 표시
	DrawDebugLine(GetWorld(), MuzzleLocation,
		bHit ? HitResults.Last().ImpactPoint : TraceEnd,
		FColor::Red, false, 2.0f);

	if (bHit)
	{
		// HitResults는 거리 순으로 정렬되어 있으므로 첫 번째가 가장 가까운 대상
		// ASC를 가진 가장 가까운 대상을 찾아 데미지 적용
		bool bDamageApplied = false;

		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor) continue;

			// 히트 대상의 ASC 조회: 캐릭터 → PlayerState → IAbilitySystemInterface
			// ASC가 PlayerState에 있으므로 HitActor(Character)가 아닌 PlayerState에서 조회
			UAbilitySystemComponent* TargetASC = nullptr;
			if (APawn* HitPawn = Cast<APawn>(HitActor))
			{
				if (APlayerState* HitPS = HitPawn->GetPlayerState())
				{
					if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(HitPS))
					{
						TargetASC = ASCInterface->GetAbilitySystemComponent();
					}
				}
			}

			const bool bHasASC = TargetASC != nullptr;

			// 디버그: 히트된 모든 액터에 구체 표시 (ASC 대상: 초록, 비대상: 노랑)
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 8.f, 8,
				bHasASC ? FColor::Green : FColor::Yellow, false, 2.0f);

			UE_LOG(LogTemp, Display, TEXT("[Fire] Hit[%d]: %s | Distance: %.0f | ASC: %s"),
				Hit.Item, *HitActor->GetName(), Hit.Distance,
				bHasASC ? TEXT("O") : TEXT("X"));

			// ASC 없거나 DamageEffectClass 미설정이면 스킵
			if (!bHasASC || !DamageEffectClass) continue;

			// GE 적용은 반드시 서버(Authority)에서만 수행.
			// 클라이언트에서 Fire()가 호출된 경우(로컬 예측 사운드/애니메이션 등) 데미지 경로는 건너뜀.
			if (!Character->HasAuthority()) break;

			// 공격자(소유 캐릭터)의 ASC는 PlayerState에 존재
			UAbilitySystemComponent* SourceASC = nullptr;
			if (ABasicPlayerState* PS = Character->GetPlayerState<ABasicPlayerState>())
			{
				SourceASC = PS->GetAbilitySystemComponent();
			}

			if (!SourceASC) break;

			// Effect Context: 누가, 어떤 무기로, 어디를 맞혔는지 기록
			FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
			ContextHandle.AddInstigator(Character, this);
			ContextHandle.AddHitResult(Hit);

			// GE Spec 생성 후 가장 가까운 ASC 대상에게 적용
			FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
				DamageEffectClass, 1.f, ContextHandle);

			if (SpecHandle.IsValid())
			{
				SourceASC->ApplyGameplayEffectSpecToTarget(
					*SpecHandle.Data.Get(), TargetASC);

				bDamageApplied = true;

				// 디버그: 데미지 적용 대상에 큰 빨간 구체
				DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 16.f, 12,
					FColor::Red, false, 3.0f);

				UE_LOG(LogTemp, Warning, TEXT("[Fire] >>> Damage applied to: %s (Distance: %.0f)"),
					*HitActor->GetName(), Hit.Distance);
			}

			// 가장 가까운 ASC 대상 1명에게만 데미지 → 루프 종료
			break;
		}

		if (!bDamageApplied)
		{
			UE_LOG(LogTemp, Display, TEXT("[Fire] %d hit(s) but no valid ASC target found"),
				HitResults.Num());
		}
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("[Fire] No hit"));
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

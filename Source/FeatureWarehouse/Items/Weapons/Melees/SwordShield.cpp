// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordShield.h"

#include "Enums/TypeOfMelee.h"

#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "NiagaraComponent.h"
#include "Perception/AISenseConfig_Hearing.h"

ASwordShield::ASwordShield()
{
	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetupAttachment(RootComponent);
	ShieldMesh->SetCollisionProfileName(FName("NoCollision"));

	SetMeleeType(ETypeOfMelee::SwordAndShield);
}

void ASwordShield::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SlashFX->SetRelativeLocation(FVector(-77.5f, 0.0f, 122.5f));

	// Set SlashFX's User Parameters.
	SlashFX->SetNiagaraVariableFloat(FString("Elemental_Lifetime"), 0.0f);
	SlashFX->SetNiagaraVariableFloat(FString("Slash_Lifetime"), 0.8f);
	SlashFX->SetNiagaraVariableFloat(FString("Slash_Noise"), 2.0f);
	SlashFX->SetNiagaraVariableFloat(FString("Slash_Width"), 80.0f);
}


void ASwordShield::TakeUp(ACharacter* NewOwner)
{
	GetTriggerZone()->SetCollisionProfileName(FName("NoCollision"));
	GetItemCollision()->SetCollisionProfileName(FName("NoCollision"));

	SetActorLocation(FVector(0.0f, 0.0f, 0.0f));
	SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
	SetActorScale3D(FVector(1.0f));

	ShieldMesh->SetRelativeLocation(FVector(0.0f));

	SetWeaponOwner(NewOwner);
}

void ASwordShield::ThrowAway(FVector Location)
{
	SetWeaponOwner(nullptr);

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Detach();

	GetItemCollision()->SetCollisionProfileName(FName("Item"));
	GetTriggerZone()->SetCollisionProfileName(FName("PlayerTrigger"));

	SetActorLocation(Location);
	SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
	ShieldMesh->SetRelativeLocation(FVector(-76.0f, 43.0f, 33.0f));

	SetActorScale3D(FVector(0.4f));
}

void ASwordShield::Attach()
{
	if (GetWeaponOwner())
	{
		BladeMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Name_SwordUnequip);
		ShieldMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Name_ShieldUnequip);
	}
}

void ASwordShield::Detach()
{
	ShieldMesh->AttachToComponent(BladeMesh, FAttachmentTransformRules::KeepRelativeTransform);
}

void ASwordShield::HoldMeleeWeapon()
{
	if (!GetWeaponOwner()) return;

	UAnimInstance* AnimInstance = GetWeaponOwner()->GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance))
	{
		if (AnimInstance->Montage_IsPlaying(MontageInfo.m_EquipMontage))
		{
			// 무기 장착
			BladeMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Name_SwordEquip);
			ShieldMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Name_ShieldEquip);
		}
		else
		{
			// 무기 장착 해제
			BladeMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Name_SwordUnequip);
			ShieldMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Name_ShieldUnequip);
		}
	}
}


void ASwordShield::AttackTrace()
{
	// 쉴드 어택
	// 모든 공격 중 약 공격을 제외하고 2회까지만 공격하기 때문에 공격 몽타주 인덱스가 3이라면 == 약공격 (쉴드 어택)
	bool IsShieldAttack = MontageIndex == 3 ? true : false;

	if (IsShieldAttack)
	{
		FVector Start = ShieldMesh->GetSocketLocation(FName("ShieldBottom"));
		FVector End = ShieldMesh->GetSocketLocation(FName("ShieldTop"));
		
		DrawAttackTrace(Start, End, IsShieldAttack);
		return;
	}
	else
	{
		FVector Start = BladeMesh->GetSocketLocation(FName("BladeBottom"));
		FVector End = BladeMesh->GetSocketLocation(FName("BladeTop"));
		FVector Midpoint = (Start + End) / 2;

		// Interpolate line trace when the distance is far.
		if (PreMidpoint != FVector::ZeroVector && FVector::Distance(PreMidpoint, Midpoint) > InterpolateDistance)
		{
			int InterpolatingNum = (int)FVector::Distance(PreMidpoint, Midpoint) / InterpolateDistance;

			float StartDelta = FVector::Distance(PreStart, Start) / InterpolatingNum;
			float StartSpeed = StartDelta;

			FVector CurBladeVector = (End - Start).GetSafeNormal();
			float BladeVectorDelta = FVector::Distance(PreBladeVector, CurBladeVector) / InterpolatingNum;
			float BladeVectorSpeed = BladeVectorDelta;

			for (int i = 0; i < InterpolatingNum; i++)
			{
				FVector InterpolatedStart = FMath::VInterpConstantTo(PreStart, Start, 1.0f, StartSpeed);
				FVector InterpolatedVector = FMath::VInterpConstantTo(PreBladeVector, CurBladeVector, 1.0f, BladeVectorSpeed);
				FVector InterpolatedEnd = InterpolatedStart + InterpolatedVector.GetSafeNormal() * BladeLength;

				DrawAttackTrace(InterpolatedStart, InterpolatedEnd, IsShieldAttack);

				StartSpeed += StartDelta;
				BladeVectorSpeed += BladeVectorDelta;
			}
		}

		PreMidpoint = Midpoint;
		PreBladeVector = (End - Start).GetSafeNormal();
		PreStart = Start;

		DrawAttackTrace(Start, End, IsShieldAttack);
	}
}

void ASwordShield::DrawAttackTrace(const FVector& LineStart, const FVector& LineEnd, bool IsShield)
{
	FHitResult Hit;
	bool IsHit;

	if (IsShield)
	{
		// 쉴드치는 경우
		IsHit = UKismetSystemLibrary::BoxTraceSingle(
			GetWorld(),
			LineStart,
			LineEnd,
			FVector(10.0f, 1.0f, 50.0f),
			FRotator::ZeroRotator,
			ETraceTypeQuery::TraceTypeQuery4,
			false,
			IgnoreActor,
			EDrawDebugTrace::None,
			Hit,
			true
		);
	}
	else
	{
		// 소드 공격일 경우
		IsHit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			LineStart,
			LineEnd,
			ETraceTypeQuery::TraceTypeQuery4,
			false,
			IgnoreActor,
			EDrawDebugTrace::None,
			Hit,
			true
		);
	}

	if (IsHit)
	{
		APawn* HitPawn = Cast<APawn>(Hit.GetActor());
		if (!IsValid(HitPawn)) return;

		IgnoreActor.Add(Hit.GetActor());
		TSubclassOf<UDamageType> DamageType;

		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, Hit.ImpactPoint);
			UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.0f, GetWeaponOwner(), 0.0f, FName("MeleeHit"));
		}

		UGameplayStatics::ApplyPointDamage(Hit.GetActor(), Damage, Hit.ImpactNormal, Hit, GetWeaponOwner()->GetController(), this, DamageType);

		ShakePlayerCamera();
	}
}

void ASwordShield::ActiveOverlay()
{
	BladeMesh->SetOverlayMaterial(OutlineMaterial);
	ShieldMesh->SetOverlayMaterial(OutlineMaterial);
}

void ASwordShield::DeactiveOverlay()
{
	BladeMesh->SetOverlayMaterial(nullptr);
	ShieldMesh->SetOverlayMaterial(nullptr);
}
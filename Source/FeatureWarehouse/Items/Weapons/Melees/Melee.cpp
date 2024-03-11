// Fill out your copyright notice in the Description page of Project Settings.


#include "Melee.h"

#include "Characters/WeaponWielder.h"
#include "Characters/PlayerCharacter.h"
#include "AnimInstance/WielderAnimInstance.h"

#include "Components/WeaponComponent.h"
#include "Components/StatComponent.h"
#include "Enums/MovementState.h"
#include "Enums/ActionState.h"

#include "Animation/AnimMontage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

AMelee::AMelee()
{
	InterpolateDistance = 20.0f;

	bIsEquip = false;
}

void AMelee::BeginPlay()
{
	Super::BeginPlay();

	FVector Start = GetSkeletalMesh()->GetSocketLocation(FName("BladeBottom"));
	FVector End = GetSkeletalMesh()->GetSocketLocation(FName("BladeTop"));
	BladeLength = (Start - End).Length();

	AttackMontages.Add(JumpAttackMontage);
	AttackMontages.Add(AttackMontage);
	AttackMontages.Add(SprintAttackMontage);
	AttackMontages.Add(HeavyAttackMontage);
}

// 무기를 장착했을 때, WeaponComponent에서 실행되는 함수.
void AMelee::BindMontage()
{
	if (!GetWeaponOwner()) return;

	AWeaponWielder* Wielder = Cast<AWeaponWielder>(GetWeaponOwner());
	if (!Wielder) return;

	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(Wielder->GetMesh()->GetAnimInstance());
	if (WielderAnim)
	{
		WielderAnim->OnMontageEnded.AddDynamic(this, &AMelee::OnAttackEnded);
		WielderAnim->OnNextAttackCheck.BindUFunction(this, FName("OnNextAttackChecked"));
		WielderAnim->OnHoldMeleeWeapon.BindUFunction(this, FName("HoldMeleeWeapon"));
		WielderAnim->OnUnequipEnd.BindUFunction(this, FName("UnequipEnded"));
		WielderAnim->OnEquipEnd.BindUFunction(this, FName("EquipEnded"));
	}
}

void AMelee::Attack()
{
	if (!CanAttack())
		return;

	if (CanCombo)
	{
		// Combo attack.
		if (!GetWeaponOwner())
			return;

		AWeaponWielder* Wielder = Cast<AWeaponWielder>(GetWeaponOwner());
		if (!Wielder) return;

		UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(Wielder->GetMesh()->GetAnimInstance());

		if (WielderAnim)
		{
			UAnimMontage* PlayMontage = FindAppropriateAttackAnimation();
			FName SectionName = PlayMontage->GetSectionName(MontageIndex);
			Wielder->PlayMontage(PlayMontage);
			WielderAnim->Montage_JumpToSection(SectionName, PlayMontage);

			MontageIndex++;
			CanCombo = false;
		}
	}
	else
	{
		// First attack.
		SetIsAttacking(true);

		AWeaponWielder* Wielder = Cast<AWeaponWielder>(GetWeaponOwner());

		if (Wielder)
		{
			UAnimMontage* PlayMontage = FindAppropriateAttackAnimation();
			Wielder->PlayMontage(PlayMontage);

			MontageIndex++;
		}
	}
}

void AMelee::Attack(EStateOfViews CurView, FVector HitLocation)
{
	if (!CanAttack())
		return;

	if (CanCombo)
	{
		// Combo attack.
		if (!GetWeaponOwner()) 
			return;

		AWeaponWielder* Wielder = Cast<AWeaponWielder>(GetWeaponOwner());
		if (!Wielder) return;

		UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(Wielder->GetMesh()->GetAnimInstance());

		if (WielderAnim)
		{
			UAnimMontage* PlayMontage = FindAppropriateAttackAnimation();
			FName SectionName = PlayMontage->GetSectionName(MontageIndex);
			Wielder->PlayMontage(PlayMontage);
			WielderAnim->Montage_JumpToSection(SectionName, PlayMontage);

			MontageIndex++;
			CanCombo = false;
		}
	}
	else
	{
		// First attack.
		SetIsAttacking(true);

		AWeaponWielder* Wielder = Cast<AWeaponWielder>(GetWeaponOwner());
		
		if (Wielder)
		{
			UAnimMontage* PlayMontage = FindAppropriateAttackAnimation();
			Wielder->PlayMontage(PlayMontage);

			MontageIndex++;
		}
	}
}

bool AMelee::CanAttack()
{
	bool CanAttack = !UKismetMathLibrary::BooleanXOR(CanCombo, IsAttacking());

	return CanAttack;
}

void AMelee::OnAttackEnded(class UAnimMontage* Montage, bool bInterrupted)
{
	if (!IsAttackMontage(Montage)) return;

	AWeaponWielder* Wielder = Cast<AWeaponWielder>(GetWeaponOwner());
	if (!IsValid(Wielder)) return;

	// 기본 공격 몽타주일 때
	if (Montage == AttackMontage)
	{
		UAnimInstance* p_AnimInstance = GetWeaponOwner()->GetMesh()->GetAnimInstance();
		if (!IsValid(p_AnimInstance)) return;

		bool IsPlaying = p_AnimInstance->Montage_IsPlaying(AttackMontage);

		if (!IsPlaying)
		{
			MontageIndex = 0;
			CanCombo = false;
			SetIsAttacking(false);
			Wielder->SetActionState(EActionState::EAS_Idle);
		}

		return;
	}

	// 강 공격 몽타주일 때
	if (Montage == HeavyAttackMontage)
	{
		UAnimInstance* p_AnimInstance = GetWeaponOwner()->GetMesh()->GetAnimInstance();
		if (!IsValid(p_AnimInstance)) return;

		bool IsPlaying = p_AnimInstance->Montage_IsPlaying(HeavyAttackMontage);

		if (!IsPlaying)
		{
			MontageIndex = 0;
			CanCombo = false;
			SetIsAttacking(false);
			Wielder->SetActionState(EActionState::EAS_Idle);
		}

		return;
	}

	// 콤보가 존재하지 않는다면 상태 초기화
	MontageIndex = 0;
	CanCombo = false;
	SetIsAttacking(false);
	Wielder->SetActionState(EActionState::EAS_Idle);
}

void AMelee::OnNextAttackChecked()
{
	if (bIsEquip) CanCombo = true;
}

void AMelee::StartAttackTrace()
{
	PreMidpoint = FVector::ZeroVector;
	PreBladeVector = FVector::ZeroVector;
	PreStart = FVector::ZeroVector;

	IgnoreActor.Add(GetWeaponOwner());

	GetWorldTimerManager().SetTimer(AttackTraceTimer, this, &AMelee::AttackTrace, 0.01f, true);
}

void AMelee::StopAttackTrace()
{
	GetWorldTimerManager().ClearTimer(AttackTraceTimer);

	IgnoreActor.Empty();

	PreMidpoint = FVector::ZeroVector;
	PreBladeVector = FVector::ZeroVector;
	PreStart = FVector::ZeroVector;
}

void AMelee::AttackTrace()
{
	FVector Start = GetSkeletalMesh()->GetSocketLocation(FName("BladeBottom"));
	FVector End = GetSkeletalMesh()->GetSocketLocation(FName("BladeTop"));
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

			DrawAttackLineTrace(InterpolatedStart, InterpolatedEnd, true);

			StartSpeed += StartDelta;
			BladeVectorSpeed += BladeVectorDelta;
		}
	}

	PreMidpoint = Midpoint;
	PreBladeVector = (End - Start).GetSafeNormal();
	PreStart = Start;

	DrawAttackLineTrace(Start, End, false);
}

void AMelee::DrawAttackLineTrace(const FVector& LineStart, const FVector& LineEnd, bool IsInterpolating)
{
	FHitResult Hit;
	bool IsHit;
		
	if (IsInterpolating)
	{
		IsHit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			LineStart,
			LineEnd,
			ETraceTypeQuery::TraceTypeQuery4,
			false,
			IgnoreActor,
			EDrawDebugTrace::ForDuration,
			Hit,
			true,
			FLinearColor::Blue,
			FLinearColor::Black,
			1.0f
		);
	}
	else
	{
		IsHit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			LineStart,
			LineEnd,
			ETraceTypeQuery::TraceTypeQuery4,
			false,
			IgnoreActor,
			EDrawDebugTrace::ForDuration,
			Hit,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			1.0f
		);
	}


	if (IsHit)
	{
		APawn* HitPawn = Cast<APawn>(Hit.GetActor());
		if (!IsValid(HitPawn)) return;

		IgnoreActor.Add(Hit.GetActor());
		TSubclassOf<UDamageType> DamageType;

		UGameplayStatics::ApplyPointDamage(Hit.GetActor(), 30.0f, Hit.ImpactNormal, Hit, HitPawn->GetController(), this, DamageType);

		ShakePlayerCamera();

		// Show Blood effet.
		/*FRotator ImpactRotation = UKismetMathLibrary::MakeRotFromZ(-Hit.ImpactNormal);
		HitActorComponent->ShowBloodEffect(Hit.ImpactPoint, ImpactRotation);*/
	}
}

UAnimMontage* AMelee::FindAppropriateAttackAnimation()
{
	AWeaponWielder* Wielder = Cast<AWeaponWielder>(GetWeaponOwner());
	if (!IsValid(Wielder)) return nullptr;

	UAnimMontage* ReturnMontage = nullptr;

	if (Wielder)
	{
		switch (Wielder->CurMovementState())
		{
		case EMovementState::EMS_Jumping:
			ReturnMontage = JumpAttackMontage;
			break;
		case EMovementState::EMS_Running:
			ReturnMontage = SprintAttackMontage;
			break;
		case EMovementState::EMS_Crouching:

			break;
		default:
			// Idle, Walking 상태일 때 약공, 강공 선택.
			ReturnMontage = (Wielder->CurActionState() == EActionState::EAS_HeavyAttacking) ? HeavyAttackMontage : AttackMontage;
			break;
		}
	}

	return ReturnMontage;
}

void AMelee::JumpAttackLanding()
{
	AWeaponWielder* Wielder = Cast<AWeaponWielder>(GetWeaponOwner());
	if (!IsValid(Wielder)) return;

	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(Wielder->GetMesh()->GetAnimInstance());
	if (IsValid(WielderAnim))
	{
		WielderAnim->Montage_Stop(1.0f, JumpAttackMontage);
		WielderAnim->Montage_Play(JumpAttackLandMontage);
	}
}

bool AMelee::IsAttackMontage(UAnimMontage* Montage)
{
	bool IsAttackMontage = AttackMontages.Contains(Montage);
	
	return IsAttackMontage;
}

void AMelee::HoldMeleeWeapon()
{

}

void AMelee::EquipEnded()
{
	AWeaponWielder* Wielder = Cast<AWeaponWielder>(GetWeaponOwner());
	if (!IsValid(Wielder)) return;

	if (Wielder->IsA<APlayerCharacter>())
	{
		// Player일 경우
	}
	else
	{
		// Wielder일 경우
		Wielder->EquipEnded();
	}
}

void AMelee::UnequipEnded()
{
	AWeaponWielder* Wielder = Cast<AWeaponWielder>(GetWeaponOwner());
	if (!IsValid(Wielder)) return;

	Wielder->SetActionState(EActionState::EAS_Idle);

	if (Wielder->IsA<APlayerCharacter>())
	{
		// Unequip이 끝났으므로, 무기 교체.
		Wielder->GetWeaponComponent()->CurEquipState() == EEquipState::SubWeapon ? Wielder->GetWeaponComponent()->EquipMainWeapon() : Wielder->GetWeaponComponent()->EquipSubWeapon();
	}
	else
	{
		Wielder->UnequipEnded();
	}
}
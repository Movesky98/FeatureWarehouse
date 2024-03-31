// Fill out your copyright notice in the Description page of Project Settings.


#include "Melee.h"

#include "Characters/WeaponWielder.h"
#include "Characters/PlayerCharacter.h"
#include "AnimInstance/WielderAnimInstance.h"

#include "Components/WeaponComponent.h"
#include "Components/StatComponent.h"

#include "Structs/MeleeInfo.h"
#include "Structs/MeleeAttackInfo.h"
#include "Structs/MeleeMontageInfo.h"

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
	
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_MeleeInfo(TEXT("/Game/Project/Data/DT_MeleeInfo"));
	if (DT_MeleeInfo.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("Melee || Load MeleeInfo DataTable Succeeded."));
		MeleeInfoTable = DT_MeleeInfo.Object;
	}

	bIsEquip = false;
}

void AMelee::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FMeleeInfo* MeleeInfoData = MeleeInfoTable->FindRow<FMeleeInfo>(WeaponName, FString("MeleeInfo Table"));
	if (MeleeInfoData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Melee || Set up MeleeInfo Data from DataTable."));
		LightAttackInfo.Append(MeleeInfoData->m_LightAttackInfo);
		HeavyAttackInfo.Append(MeleeInfoData->m_HeavyAttackInfo);
		JumpAttackInfo = MeleeInfoData->m_JumpAttackInfo;
		SprintAttackInfo = MeleeInfoData->m_SprintAttackInfo;
		MontageInfo = MeleeInfoData->m_MontageInfo;
		
		bHasEquipMontage = true;

		AttackMontages.Add(MontageInfo.m_HeavyAttackMontage);
		AttackMontages.Add(MontageInfo.m_JumpAttackMontage);
		AttackMontages.Add(MontageInfo.m_LightAttackMontage);
		AttackMontages.Add(MontageInfo.m_SprintAttackMontage);
	}

	FVector Start = GetSkeletalMesh()->GetSocketLocation(FName("BladeBottom"));
	FVector End = GetSkeletalMesh()->GetSocketLocation(FName("BladeTop"));
	BladeLength = (Start - End).Length();
}

void AMelee::BeginPlay()
{
	Super::BeginPlay();

}

void AMelee::SaveDodgeMontages(TMap<EDirection, UAnimMontage*>& Montages)
{
	if (!MontageInfo.m_DodgeMontages.IsEmpty())
	{
		Montages = MontageInfo.m_DodgeMontages;
	}
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
		WielderAnim->OnMontageEnded.AddDynamic(this, &AMelee::OnEquipEnded);
		WielderAnim->OnMontageEnded.AddDynamic(this, &AMelee::OnUnequipEnded);
		WielderAnim->OnNextAttackCheck.BindUFunction(this, FName("OnNextAttackChecked"));
		WielderAnim->OnHoldMeleeWeapon.BindUFunction(this, FName("HoldMeleeWeapon"));

		UE_LOG(LogTemp, Warning, TEXT("Weapon - %s :: BindMontage is called."), *UKismetSystemLibrary::GetDisplayName(this));
	}
}

void AMelee::UnbindMontage()
{
	if (!GetWeaponOwner()) return;

	AWeaponWielder* Wielder = Cast<AWeaponWielder>(GetWeaponOwner());
	if (!Wielder) return;

	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(Wielder->GetMesh()->GetAnimInstance());
	if (WielderAnim)
	{
		WielderAnim->OnMontageEnded.Clear();
		WielderAnim->OnNextAttackCheck.Unbind();
		WielderAnim->OnHoldMeleeWeapon.Unbind();


		UE_LOG(LogTemp, Warning, TEXT("Weapon - %s :: UnbindMontage is called."), *UKismetSystemLibrary::GetDisplayName(this));
	}
}

void AMelee::Attack()
{
	if (!bIsEquip || !CanAttack())
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
			UAnimMontage* PlayMontage = FindAppropriateAttackAnimationWithDamage(Damage, MontageIndex);
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
			UAnimMontage* PlayMontage = FindAppropriateAttackAnimationWithDamage(Damage, MontageIndex);
			Wielder->PlayMontage(PlayMontage);

			MontageIndex++;
		}
	}
}

void AMelee::Attack(EStateOfViews CurView, FVector HitLocation)
{
	if (!bIsEquip || !CanAttack())
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
			UAnimMontage* PlayMontage = FindAppropriateAttackAnimationWithDamage(Damage, MontageIndex);
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
			UAnimMontage* PlayMontage = FindAppropriateAttackAnimationWithDamage(Damage, MontageIndex);
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
	if (Montage == MontageInfo.m_LightAttackMontage)
	{
		UAnimInstance* p_AnimInstance = GetWeaponOwner()->GetMesh()->GetAnimInstance();
		if (!IsValid(p_AnimInstance)) return;

		bool IsPlaying = p_AnimInstance->Montage_IsPlaying(MontageInfo.m_LightAttackMontage);

		if (!IsPlaying)
		{
			MontageIndex = 0;
			CanCombo = false;
			SetIsAttacking(false);

			Wielder->CurActionState() == EActionState::EAS_Attacking ? Wielder->SetActionState(EActionState::EAS_Idle) : nullptr;
		}

		return;
	}

	// 강 공격 몽타주일 때
	if (Montage == MontageInfo.m_HeavyAttackMontage)
	{
		UAnimInstance* p_AnimInstance = GetWeaponOwner()->GetMesh()->GetAnimInstance();
		if (!IsValid(p_AnimInstance)) return;

		bool IsPlaying = p_AnimInstance->Montage_IsPlaying(MontageInfo.m_HeavyAttackMontage);

		if (!IsPlaying)
		{
			MontageIndex = 0;
			CanCombo = false;
			SetIsAttacking(false);
			Wielder->CurActionState() == EActionState::EAS_HeavyAttacking ? Wielder->SetActionState(EActionState::EAS_Idle) : nullptr;
		}

		return;
	}

	// 콤보가 존재하지 않는다면 상태 초기화 (SprintAttack, JumpAttack)
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

			DrawAttackLineTrace(InterpolatedStart, InterpolatedEnd);

			StartSpeed += StartDelta;
			BladeVectorSpeed += BladeVectorDelta;
		}
	}

	PreMidpoint = Midpoint;
	PreBladeVector = (End - Start).GetSafeNormal();
	PreStart = Start;

	DrawAttackLineTrace(Start, End);
}

void AMelee::DrawAttackLineTrace(const FVector& LineStart, const FVector& LineEnd)
{
	FHitResult Hit;
	bool IsHit = UKismetSystemLibrary::LineTraceSingle(
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


	if (IsHit)
	{
		APawn* HitPawn = Cast<APawn>(Hit.GetActor());
		if (!IsValid(HitPawn)) return;

		IgnoreActor.Add(Hit.GetActor());
		TSubclassOf<UDamageType> DamageType;

		UGameplayStatics::ApplyPointDamage(Hit.GetActor(), Damage, Hit.ImpactNormal, Hit, GetWeaponOwner()->GetController(), this, DamageType);

		if(Hit.GetActor()->IsA<APlayerCharacter>())
			ShakePlayerCamera();
	}
}

UAnimMontage* AMelee::FindAppropriateAttackAnimationWithDamage(float& Change_Damage, int Index)
{
	AWeaponWielder* Wielder = Cast<AWeaponWielder>(GetWeaponOwner());
	if (!IsValid(Wielder)) return nullptr;

	UAnimMontage* ReturnMontage = nullptr;

	if (Wielder)
	{
		switch (Wielder->CurMovementState())
		{
		case EMovementState::EMS_Jumping:
			ReturnMontage = MontageInfo.m_JumpAttackMontage;
			Change_Damage = JumpAttackInfo.m_Damage;
			break;
		case EMovementState::EMS_Running:
			ReturnMontage = MontageInfo.m_SprintAttackMontage;
			Change_Damage = SprintAttackInfo.m_Damage;
			break;
		case EMovementState::EMS_Crouching:

			break;
		default:
			// Idle, Walking 상태일 때 약공, 강공 선택.
			if (Wielder->CurActionState() == EActionState::EAS_HeavyAttacking)
			{
				ReturnMontage = MontageInfo.m_HeavyAttackMontage;
				Change_Damage = HeavyAttackInfo[Index].m_Damage;

			}
			else
			{
				ReturnMontage = MontageInfo.m_LightAttackMontage;
				Change_Damage = LightAttackInfo[Index].m_Damage;
			}
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
		WielderAnim->Montage_Stop(1.0f, MontageInfo.m_JumpAttackMontage);
		WielderAnim->Montage_Play(MontageInfo.m_JumpAttackLand);
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
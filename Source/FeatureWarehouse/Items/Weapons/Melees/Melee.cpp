// Fill out your copyright notice in the Description page of Project Settings.


#include "Melee.h"

#include "Characters/PlayerCharacter.h"
#include "Characters/Dummy.h"
#include "AnimInstance/PlayerAnimInstance.h"
#include "Components/StatComponent.h"

#include "Animation/AnimMontage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

AMelee::AMelee()
{
	InterpolateDistance = 20.0f;
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
}

// 무기를 장착했을 때, WeaponComponent에서 실행되는 함수.
void AMelee::BindMontage()
{
	if (!GetWeaponOwner()) return;

	APlayerCharacter* Player = Cast<APlayerCharacter>(GetWeaponOwner());
	if (!Player) return;

	UPlayerAnimInstance* PlayerAnim = Cast<UPlayerAnimInstance>(Player->GetMesh()->GetAnimInstance());

	if (PlayerAnim)
	{
		PlayerAnim->OnMontageEnded.AddDynamic(this, &AMelee::OnAttackEnded);
		PlayerAnim->OnNextAttackCheck.AddUFunction(this, FName("OnNextAttackChecked"));
		PlayerAnim->OnHoldMeleeWeapon.BindUFunction(this, FName("HoldMeleeWeapon"));
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

		APlayerCharacter* Player = Cast<APlayerCharacter>(GetWeaponOwner());
		if (!Player) 
			return;

		UPlayerAnimInstance* PlayerAnim = Cast<UPlayerAnimInstance>(Player->GetMesh()->GetAnimInstance());

		if (PlayerAnim)
		{
			UAnimMontage* PlayMontage = FindAppropriateAttackAnimation();
			FName SectionName = PlayMontage->GetSectionName(MontageIndex);
			Player->PlayMontage(PlayMontage);
			PlayerAnim->Montage_JumpToSection(SectionName, PlayMontage);

			MontageIndex++;
			CanCombo = false;
		}
	}
	else
	{
		// First attack.
		SetIsAttacking(true);

		APlayerCharacter* Player = Cast<APlayerCharacter>(GetWeaponOwner());
		if (Player)
		{
			UAnimMontage* PlayMontage = FindAppropriateAttackAnimation();
			Player->PlayMontage(PlayMontage);

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

	// 콤보가 존재하는 공격 몽타주일 때
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
		}

		return;
	}

	// 콤보가 존재하지 않는다면 상태 초기화
	MontageIndex = 0;
	CanCombo = false;
	SetIsAttacking(false);
}

void AMelee::OnNextAttackChecked()
{
	CanCombo = true;
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
		UStatComponent* HitActorComponent = Cast<UStatComponent>(Hit.GetActor()->GetComponentByClass(UStatComponent::StaticClass()));

		if (HitActorComponent)
		{
			IgnoreActor.Add(Hit.GetActor());

			HitActorComponent->GetDamaged(30.0f);

			ShakePlayerCamera();

			// Show Blood effet.
			FRotator ImpactRotation = UKismetMathLibrary::MakeRotFromZ(-Hit.ImpactNormal);
			HitActorComponent->ShowBloodEffect(Hit.ImpactPoint, ImpactRotation);
		}
	}
}

UAnimMontage* AMelee::FindAppropriateAttackAnimation()
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(GetWeaponOwner());
	UAnimMontage* ReturnMontage = nullptr;

	if (Character)
	{
		switch (Character->GetMovementState())
		{
		case EMovementState::Jumping:
			ReturnMontage = JumpAttackMontage;
			break;
		case EMovementState::Running:
			ReturnMontage = SprintAttackMontage;
			break;
		case EMovementState::Crouching:

			break;
		default:
			ReturnMontage = AttackMontage;
			break;
		}
	}

	return ReturnMontage;
}

void AMelee::JumpAttackLanding()
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetWeaponOwner());
	if (!Player)
		return;

	UPlayerAnimInstance* PlayerAnim = Cast<UPlayerAnimInstance>(Player->GetMesh()->GetAnimInstance());
	if (IsValid(PlayerAnim))
	{
		PlayerAnim->Montage_Stop(1.0f, JumpAttackMontage);
		PlayerAnim->Montage_Play(JumpAttackLandMontage);
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
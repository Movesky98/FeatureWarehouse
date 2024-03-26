// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"
#include "Characters/WeaponWielder.h"
#include "AnimInstance/WielderAnimInstance.h"

#include "Enums/ActionState.h"

#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GamePlayStatics.h"
#include "Particles/ParticleSystem.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_Blood(TEXT("/Game/ImportAssets/Realistic_Starter_VFX_Pack_Vol2/Particles/Blood/P_Blood_Splat_Cone"));
	if (P_Blood.Succeeded())
	{
		BloodParticle = P_Blood.Object;
	}

	StunGauge = 0.0f;
	DamagableType = EDamagableType::EDT_VULNERABLE;
}

// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	AWeaponWielder* WeaponWielder = Cast<AWeaponWielder>(GetOwner());

	if (!IsValid(WeaponWielder)) return;

	UWielderAnimInstance* AnimInstance = Cast<UWielderAnimInstance>(WeaponWielder->GetMesh()->GetAnimInstance());

	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &UStatComponent::OnGetDamagedEnded);
		AnimInstance->OnDeathEnd.BindUFunction(this, FName("OnDeathEnded"));
	}
}

void UStatComponent::OnDeathEnded()
{
	AWeaponWielder* WeaponWielder = Cast<AWeaponWielder>(GetOwner());

	if (IsValid(WeaponWielder))
	{
		WeaponWielder->Die();
	}
}

void UStatComponent::OnGetDamagedEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != GetDamagedMontage) return;
	
	AWeaponWielder* WeaponWielder = Cast<AWeaponWielder>(GetOwner());
	if (!IsValid(WeaponWielder)) return;

	UAnimInstance* Anim = WeaponWielder->GetMesh()->GetAnimInstance();

	// 피격 몽타주가 실행되지 않을 때에 상태 변경
	if (Anim && !Anim->Montage_IsPlaying(GetDamagedMontage))
	{
		WeaponWielder->SetActionState(EActionState::EAS_Idle);

		if (!OnGetDamaged.IsBound()) return;

		// 피격 델리게이트 콜백 함수 호출.
		(DamageAccumulation > MaxHP * 0.3f) ? OnGetDamaged.Execute(true) : OnGetDamaged.Execute(false);
		DamageAccumulation = 0.0f;
	}
}

void UStatComponent::SetMontages(TMap<FString, UAnimMontage*> AnimMontages)
{
	for (auto& AnimMontage : AnimMontages)
	{
		FString MontageString = AnimMontage.Key;

		if (MontageString.Equals(FString("GetDamagedMontage")))
		{
			GetDamagedMontage = IsValid(AnimMontage.Value) ? AnimMontage.Value : nullptr;
			continue;
		}

		if (MontageString.Equals(FString("DeathMontage")))
		{
			DeathMontage = IsValid(AnimMontage.Value) ? AnimMontage.Value : nullptr;
			continue;
		}

		if (MontageString.Equals(FString("KnockdownMontage")))
		{

			continue;
		}

		if (MontageString.Equals(FString("RetreatMontage")))
		{
			RetreatMontage = IsValid(AnimMontage.Value) ? AnimMontage.Value : nullptr;
			continue;
		}
	}
}

void UStatComponent::DecreaseHP(float Damage)
{
	// 데미지를 받을 수 있는 상태가 아니라면 HP를 깎지 않음.
	if (DamagableType != EDamagableType::EDT_VULNERABLE) return;

	AWeaponWielder* WeaponWielder = Cast<AWeaponWielder>(GetOwner());
	if (!IsValid(WeaponWielder)) return;
	
	if (CurrentHP <= 0.0f)
	{
		WeaponWielder->SetIsDead(true);
		IsValid(DeathMontage) ? PlayMontage(DeathMontage) : WeaponWielder->Die();
		return;
	}

	CurrentHP -= Damage;

	// 피격 몽타주가 없으면 바로 상태 변경
	IsValid(GetDamagedMontage) ? PlayMontage(GetDamagedMontage) : WeaponWielder->SetActionState(EActionState::EAS_Idle);

	DamageAccumulation += Damage;
}

void UStatComponent::ShowBloodEffect(FVector Location, FRotator Rotation)
{
	UWorld* World = GetWorld();
	if (World)
	{
		UGameplayStatics::SpawnEmitterAtLocation(World, BloodParticle, Location, Rotation, true);
	}
}

void UStatComponent::PlayMontage(UAnimMontage* PlayMontage)
{
	AWeaponWielder* WeaponWielder = Cast<AWeaponWielder>(GetOwner());

	UAnimInstance* OwnerAnim = WeaponWielder->GetMesh()->GetAnimInstance();
	if (OwnerAnim)
	{
		// 재생되고 있는 몽타주 중지.
		if(OwnerAnim->IsAnyMontagePlaying())
			OwnerAnim->StopAllMontages(0.0f);

		// 몽타주 재생 (피격, 죽음)
		OwnerAnim->Montage_Play(PlayMontage);
	}
}

void UStatComponent::SetDamagableType(EDamagableType Type)
{
	if(DamagableType != EDamagableType::EDT_INVINCIBLE)
		DamagableType = Type;
}
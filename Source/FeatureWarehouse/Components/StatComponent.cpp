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
	}
}

void UStatComponent::OnDeathEnded(UAnimMontage* Montage, bool bInterrupted)
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

	// �ǰ� ��Ÿ�ְ� ������� ���� ���� ���� ����
	if (Anim && !Anim->Montage_IsPlaying(GetDamagedMontage))
	{
		WeaponWielder->SetActionState(EActionState::EAS_Idle);

		if (!OnGetDamaged.IsBound()) return;

		// �ǰ� ��������Ʈ �ݹ� �Լ� ȣ��.
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
	// �������� ���� �� �ִ� ���°� �ƴ϶�� HP�� ���� ����.
	if (DamagableType != EDamagableType::EDT_VULNERABLE) return;

	AWeaponWielder* WeaponWielder = Cast<AWeaponWielder>(GetOwner());
	if (!IsValid(WeaponWielder)) return;
	
	if (CurrentHP <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Set %s's dead state."), *UKismetSystemLibrary::GetDisplayName(WeaponWielder));
		WeaponWielder->SetIsDead(true);
		IsValid(DeathMontage) ? PlayDeathMontage() : WeaponWielder->Die();
		return;
	}

	CurrentHP -= Damage;

	// �ǰ� ��Ÿ�ְ� ������ �ٷ� ���� ����
	IsValid(GetDamagedMontage) ? PlayMontage(GetDamagedMontage) : WeaponWielder->SetActionState(EActionState::EAS_Idle);

	DamageAccumulation += Damage;
}

void UStatComponent::DecreaseStamina(float Amount)
{
	UE_LOG(LogTemp, Warning, TEXT("Decrase Stamina!"));
	if (CurrentStamina - Amount < 0.0f)
	{
		CurrentStamina = 0.0f;
		bIsExhaustedStamina = true;

		AWeaponWielder* WielderOwner = Cast<AWeaponWielder>(GetOwner());
		if(WielderOwner) WielderOwner->SetActionState(EActionState::EAS_Exhausting);

		if (bIsStartRecoveryStaminaWaitTimer) StopRecoveryStamina();

		StartRecoveryStamina(RecoveryStaminaWaitTime);

		if (OnExhaustedStamina.IsBound())
		{
			OnExhaustedStamina.Execute();
		}
	}

	CurrentStamina -= Amount;

	if (bIsStartRecoveryStamina)
	{
		// ���¹̳� ȸ�� ��, ���¹̳��� ���� ���� ���� ��� ȸ�� ����
		StopRecoveryStamina();

		// �ð� ����� �����ϱ� ���� �� ����.
		bIsStartRecoveryStaminaWaitTimer = false;
		bIsStartRecoveryStamina = false;
	}

	// �ð� ��� ���� üũ
	if (!bIsStartRecoveryStaminaWaitTimer && !bIsStartRecoveryStamina)
	{
		bIsStartRecoveryStaminaWaitTimer = true;
		GetWorld()->GetTimerManager().SetTimer(RecoveryStaminaTimer, this, &UStatComponent::CalculateStaminaWaitTime, 1.0f, true);
	}
	else
	{
		// ���������� ���¹̳� �Ҹ��� �� ���� ��� �ð� �ʱ�ȭ
		StaminaElapsedTime = 0.0f;
	}
}

void UStatComponent::StartRecoveryStamina(float WaitTime)
{
	GetWorld()->GetTimerManager().SetTimer(RecoveryStaminaTimer, this, &UStatComponent::RecoveryStamina, 0.01f, true, WaitTime);
}

void UStatComponent::StopRecoveryStamina()
{
	GetWorld()->GetTimerManager().ClearTimer(RecoveryStaminaTimer);
}

void UStatComponent::CalculateStaminaWaitTime()
{
	StaminaElapsedTime += 1.0f;

	// ����� �ð��� �ȴٸ�
	if (RecoveryStaminaWaitTime <= StaminaElapsedTime)
	{
		GetWorld()->GetTimerManager().ClearTimer(RecoveryStaminaTimer);
		bIsStartRecoveryStamina = true;
		StartRecoveryStamina();
	}
}

void UStatComponent::RecoveryStamina()
{
	CurrentStamina += MaxStamina * 0.01f;

	// ��ģ �������� üũ
	if (bIsExhaustedStamina)
	{
		if (CurrentStamina > MaxStamina * 0.5f)
		{
			// Stamina�� ���� �Ѿ��ٸ�, ��ģ ���¸� �����ϰ� ���� �׼��� ���� �� �ֵ��� Idle�� �ٲ���.
			bIsExhaustedStamina = false;

			AWeaponWielder* WielderOwner = Cast<AWeaponWielder>(GetOwner());
			if(WielderOwner) WielderOwner->SetActionState(EActionState::EAS_Idle);
		}
	}

	if (CurrentStamina >= MaxStamina)
	{
		CurrentStamina = MaxStamina;
		bIsStartRecoveryStaminaWaitTimer = false;
		bIsStartRecoveryStamina = false;
		StopRecoveryStamina();
	}
}

void UStatComponent::ShowBloodEffect(FVector Location, FRotator Rotation)
{
	UWorld* World = GetWorld();
	if (World)
	{
		UGameplayStatics::SpawnEmitterAtLocation(World, BloodParticle, Location, Rotation, true);
	}
}

void UStatComponent::PlayDeathMontage()
{
	AWeaponWielder* WeaponWielder = Cast<AWeaponWielder>(GetOwner());
	if (!WeaponWielder) return;

	UAnimInstance* OwnerAnim = WeaponWielder->GetMesh()->GetAnimInstance();
	if (OwnerAnim)
	{
		UE_LOG(LogTemp, Warning, TEXT("Play %s's DeathMontage."), *UKismetSystemLibrary::GetDisplayName(WeaponWielder));
		bool bPlayedSuccessfully = false;
		const float MontageLength = OwnerAnim->Montage_Play(DeathMontage);

		bPlayedSuccessfully = MontageLength > 0.f;

		if (bPlayedSuccessfully)
		{
			UE_LOG(LogTemp, Warning, TEXT("Bind the function when %s's DeathMontage is ended."), *UKismetSystemLibrary::GetDisplayName(WeaponWielder));
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &UStatComponent::OnDeathEnded);
			OwnerAnim->Montage_SetEndDelegate(EndDelegate, DeathMontage);
		}
	}
}

void UStatComponent::PlayMontage(UAnimMontage* PlayMontage)
{
	AWeaponWielder* WeaponWielder = Cast<AWeaponWielder>(GetOwner());

	UAnimInstance* OwnerAnim = WeaponWielder->GetMesh()->GetAnimInstance();
	if (OwnerAnim)
	{
		// ����ǰ� �ִ� ��Ÿ�� ����.
		if(OwnerAnim->IsAnyMontagePlaying())
			OwnerAnim->StopAllMontages(0.0f);

		// ��Ÿ�� ��� (�ǰ�, ����)
		OwnerAnim->Montage_Play(PlayMontage);
	}
}

void UStatComponent::SetDamagableType(EDamagableType Type)
{
	if(DamagableType != EDamagableType::EDT_INVINCIBLE)
		DamagableType = Type;
}
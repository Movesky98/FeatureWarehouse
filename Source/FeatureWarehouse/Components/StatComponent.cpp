// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"
#include "Characters/WeaponWielder.h"
#include "AnimInstance/WielderAnimInstance.h"

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
	// 회피 델리게이트를 실행시킴.
	if (Montage == GetDamagedMontage)
	{
		OnRetreatFromEnemy.IsBound() ? OnRetreatFromEnemy.Execute() : nullptr;
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
	if (!bIsDamagable) return;

	CurrentHP -= Damage;

	if (GetDamagedMontage)
	{
		PlayMontage(GetDamagedMontage);
	}

	AWeaponWielder* WeaponWielder = Cast<AWeaponWielder>(GetOwner());
	if (!IsValid(WeaponWielder)) return;

	if (CurrentHP <= 0.0f)
	{
		IsValid(DeathMontage) ? PlayMontage(DeathMontage) : WeaponWielder->Die();
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

void UStatComponent::PlayMontage(UAnimMontage* PlayMontage)
{
	USkeletalMeshComponent* OwnerSkeletalMesh = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	
	if (!OwnerSkeletalMesh) return;
	
	UAnimInstance* OwnerAnim = OwnerSkeletalMesh->GetAnimInstance();
	if (OwnerAnim)
	{
		// 재생되고 있는 몽타주 중지.
		OwnerAnim->Montage_Stop(0.0f, nullptr);

		// 몽타주 재생 (피격, 죽음)
		OwnerAnim->Montage_Play(PlayMontage);
	}
	
}
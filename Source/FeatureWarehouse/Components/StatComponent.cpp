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
		AnimInstance->OnDeathEnd.BindUFunction(this, FName("OnDeathEnded"));
	}
}

void UStatComponent::OnDeathEnded()
{
	ACharacter* OwnerCharacter = Cast<AWeaponWielder>(GetOwner());

	if (IsValid(OwnerCharacter))
	{
		OwnerCharacter->GetCapsuleComponent()->SetCollisionProfileName(FName("Ragdoll"));
		OwnerCharacter->GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
		OwnerCharacter->GetMesh()->SetSimulatePhysics(true);
	}
}

void UStatComponent::SetMontages(UAnimMontage* p_GetDamagedMontage, UAnimMontage* p_DeathMontage)
{
	GetDamagedMontage = IsValid(p_GetDamagedMontage) ? p_GetDamagedMontage : nullptr;
	DeathMontage = IsValid(p_DeathMontage) ? p_DeathMontage : nullptr;
}

bool UStatComponent::GetDamaged(float Damage)
{
	if (!bIsDamagable) return false;

	CurrentHP -= Damage;

	if (GetDamagedMontage)
	{
		PlayMontage(GetDamagedMontage);
	}

	if (CurrentHP <= 0.0f)
	{
		ACharacter* OwnerCharacter = Cast<AWeaponWielder>(GetOwner());

		if (IsValid(OwnerCharacter))
		{
			if (DeathMontage)
			{
				PlayMontage(DeathMontage);
			}
			else
			{
				OwnerCharacter->GetCapsuleComponent()->SetCollisionProfileName(FName("Ragdoll"));
				OwnerCharacter->GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
				OwnerCharacter->GetMesh()->SetSimulatePhysics(true);
			}

			return true;
		}
	}

	return true;
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
// Fill out your copyright notice in the Description page of Project Settings.


#include "Melees/Katana.h"
#include "AnimInstance/PlayerAnimInstance.h"
#include "Enums/TypeOfMelee.h"

#include "GameFramework/Character.h"

AKatana::AKatana()
{
	Scabbard = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Scabbard"));
	Scabbard->SetCollisionProfileName(FName("NoCollision"));
	Scabbard->SetupAttachment(RootComponent);

	SetMeleeType(ETypeOfMelee::Katana);
}

// īŸ���� ���� �� ���̴� �Լ�.
void AKatana::Equip()
{
	if (GetWeaponOwner() && IsValid(MontageInfo.m_EquipMontage))
	{
		UAnimInstance* AnimInstance = GetWeaponOwner()->GetMesh()->GetAnimInstance();
		if (IsValid(AnimInstance))
		{
			// �޼տ� Į��, Į���� ���������.
			GetSkeletalMesh()->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_LeftHandSocketName);
			Scabbard->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_LeftHandSocketName);
			AnimInstance->Montage_Play(MontageInfo.m_EquipMontage);
		}
	}
}

void AKatana::HoldMeleeWeapon()
{
	if (!GetWeaponOwner()) return;

	UAnimInstance* AnimInstance = GetWeaponOwner()->GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance))
	{
		if (AnimInstance->Montage_IsPlaying(MontageInfo.m_EquipMontage))
		{
			// �� ���ʿ� Į��, ���� �տ� Į��.
			GetSkeletalMesh()->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_RightHandSocketName);
			Scabbard->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_UnequipSocketName);
		}
		else
		{
			// �� �� �� ���ʿ� ����ġ.
			GetSkeletalMesh()->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_UnequipSocketName);
			Scabbard->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_UnequipSocketName);
		}
	}
}

void AKatana::Unequip()
{
	if (GetWeaponOwner() && IsValid(MontageInfo.m_UnequipMontage))
	{
		UAnimInstance* AnimInstance = GetWeaponOwner()->GetMesh()->GetAnimInstance();
		if (IsValid(AnimInstance))
		{
			// �޼տ� Į��, �����տ� īŸ���� ���������.
			GetSkeletalMesh()->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_RightHandSocketName);
			Scabbard->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_LeftHandSocketName);
			AnimInstance->Montage_Play(MontageInfo.m_UnequipMontage);
		}
	}
}

void AKatana::Attach()
{
	if (GetWeaponOwner())
	{
		// �� �� �� ���ʿ� ����ġ.
		GetSkeletalMesh()->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_UnequipSocketName);
		Scabbard->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_UnequipSocketName);
	}
}

void AKatana::Detach()
{
	Scabbard->AttachToComponent(GetSkeletalMesh(), FAttachmentTransformRules::KeepRelativeTransform);
}

void AKatana::ActiveOverlay()
{
	GetSkeletalMesh()->SetOverlayMaterial(OutlineMaterial);
	Scabbard->SetOverlayMaterial(OutlineMaterial);
}

void AKatana::DeactiveOverlay()
{
	GetSkeletalMesh()->SetOverlayMaterial(nullptr);
	Scabbard->SetOverlayMaterial(nullptr);
}
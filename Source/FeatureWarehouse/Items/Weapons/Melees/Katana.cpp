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

// Ä«Å¸³ª¸¦ ²¨³¾ ¶§ ¾²ÀÌ´Â ÇÔ¼ö.
void AKatana::Equip()
{
	if (GetWeaponOwner() && IsValid(EquipMontage))
	{
		UAnimInstance* AnimInstance = GetWeaponOwner()->GetMesh()->GetAnimInstance();
		if (IsValid(AnimInstance))
		{
			// ¿Þ¼Õ¿¡ Ä®Áý, Ä®³¯À» º¸¿©Áà¾ßÇÔ.
			GetSkeletalMesh()->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_LeftHandSocketName);
			Scabbard->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_LeftHandSocketName);
			AnimInstance->Montage_Play(EquipMontage);
		}
	}
}

void AKatana::HoldMeleeWeapon()
{
	if (!GetWeaponOwner()) return;

	UAnimInstance* AnimInstance = GetWeaponOwner()->GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance))
	{
		if (AnimInstance->Montage_IsPlaying(EquipMontage))
		{
			// ¸ö ¿ÞÂÊ¿¡ Ä®Áý, ¿À¸¥ ¼Õ¿¡ Ä®³¯.
			GetSkeletalMesh()->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_RightHandSocketName);
			Scabbard->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_UnequipSocketName);
		}
		else
		{
			// µÑ ´Ù ¸ö ¿ÞÂÊ¿¡ ¾îÅÂÄ¡.
			GetSkeletalMesh()->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_UnequipSocketName);
			Scabbard->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_UnequipSocketName);
		}
	}
}

void AKatana::Unequip()
{
	if (GetWeaponOwner() && IsValid(UnequipMontage))
	{
		UAnimInstance* AnimInstance = GetWeaponOwner()->GetMesh()->GetAnimInstance();
		if (IsValid(AnimInstance))
		{
			// ¿Þ¼Õ¿¡ Ä®Áý, ¿À¸¥¼Õ¿¡ Ä«Å¸³ª¸¦ º¸¿©Áà¾ßÇÔ.
			GetSkeletalMesh()->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_RightHandSocketName);
			Scabbard->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_LeftHandSocketName);
			AnimInstance->Montage_Play(UnequipMontage);
		}
	}
}

void AKatana::Attach()
{
	if (GetWeaponOwner())
	{
		// µÑ ´Ù ¸ö ¿ÞÂÊ¿¡ ¾îÅÂÄ¡.
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
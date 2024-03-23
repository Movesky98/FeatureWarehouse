// Fill out your copyright notice in the Description page of Project Settings.


#include "Halberd.h"

#include "AnimInstance/PlayerAnimInstance.h"
#include "GameFramework/Character.h"


void AHalberd::Equip()
{
	if (GetWeaponOwner() && IsValid(EquipMontage))
	{
		UAnimInstance* OwnerAnim = GetWeaponOwner()->GetMesh()->GetAnimInstance();
		if (OwnerAnim)
		{
			OwnerAnim->Montage_Play(EquipMontage);
		}
	}
}

void AHalberd::Unequip()
{
	if (GetWeaponOwner() && IsValid(UnequipMontage))
	{
		UAnimInstance* OwnerAnim = GetWeaponOwner()->GetMesh()->GetAnimInstance();
		if (OwnerAnim)
		{
			OwnerAnim->Montage_Play(UnequipMontage);
		}
	}
}

void AHalberd::Attach()
{
	if (GetWeaponOwner())
	{
		AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, UnequipSocketName);
	}
}

void AHalberd::HoldMeleeWeapon()
{
	if (!GetWeaponOwner()) return;

	UAnimInstance* AnimInstance = GetWeaponOwner()->GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance))
	{
		if (AnimInstance->Montage_IsPlaying(EquipMontage))
		{
			AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, EquipSocketName);
		}
		else
		{
			AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, UnequipSocketName);
		}
	}
}
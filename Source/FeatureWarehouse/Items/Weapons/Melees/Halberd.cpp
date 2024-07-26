// Fill out your copyright notice in the Description page of Project Settings.


#include "Halberd.h"

#include "Enums/TypeOfMelee.h"

#include "AnimInstance/PlayerAnimInstance.h"
#include "GameFramework/Character.h"

#include "NiagaraComponent.h"


AHalberd::AHalberd()
{
	SetMeleeType(ETypeOfMelee::Halberd);
}

void AHalberd::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SlashFX->SetRelativeLocation(FVector(67.0f, 37.0f, 0.0f));

	// Set SlashFX's User Parameters.
	SlashFX->SetNiagaraVariableFloat(FString("Elemental_Lifetime"), 0.0f);
	SlashFX->SetNiagaraVariableFloat(FString("Slash_Lifetime"), 1.0f);
	SlashFX->SetNiagaraVariableFloat(FString("Slash_Noise"), 3.0f);
	SlashFX->SetNiagaraVariableFloat(FString("Slash_Width"), 120.0f);
}

void AHalberd::Equip()
{
	if (GetWeaponOwner() && IsValid(MontageInfo.m_EquipMontage))
	{
		UAnimInstance* OwnerAnim = GetWeaponOwner()->GetMesh()->GetAnimInstance();
		if (OwnerAnim)
		{
			OwnerAnim->Montage_Play(MontageInfo.m_EquipMontage);
		}
	}
}

void AHalberd::Unequip()
{
	if (GetWeaponOwner() && IsValid(MontageInfo.m_UnequipMontage))
	{
		UAnimInstance* OwnerAnim = GetWeaponOwner()->GetMesh()->GetAnimInstance();
		if (OwnerAnim)
		{
			OwnerAnim->Montage_Play(MontageInfo.m_UnequipMontage);
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
		if (AnimInstance->Montage_IsPlaying(MontageInfo.m_EquipMontage))
		{
			AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, EquipSocketName);
		}
		else
		{
			AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, UnequipSocketName);
		}
	}
}
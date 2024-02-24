// Fill out your copyright notice in the Description page of Project Settings.


#include "Melees/Katana.h"
#include "AnimInstance/PlayerAnimInstance.h"

#include "GameFramework/Character.h"

AKatana::AKatana()
{
	Scabbard = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Scabbard"));
	Scabbard->SetCollisionProfileName(FName("NoCollision"));
	Scabbard->SetupAttachment(RootComponent);
}

// īŸ���� ���� �� ���̴� �Լ�.
void AKatana::Equip()
{
	if (GetWeaponOwner() && IsValid(EquipMontage))
	{
		UAnimInstance* AnimInstance = GetWeaponOwner()->GetMesh()->GetAnimInstance();
		if (IsValid(AnimInstance))
		{
			// �޼տ� Į��, Į���� ���������.
			GetSkeletalMesh()->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_LeftHandSocketName);
			Scabbard->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_LeftHandSocketName);
			AnimInstance->Montage_Play(EquipMontage);
		}
	}
}

void AKatana::HoldMeleeWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("Hold Melee Weapon Anim Notify is called."));
	if (!GetWeaponOwner()) return;

	UAnimInstance* AnimInstance = GetWeaponOwner()->GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance))
	{
		if (AnimInstance->Montage_IsPlaying(EquipMontage))
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
	if (GetWeaponOwner() && IsValid(UnequipMontage))
	{
		UAnimInstance* AnimInstance = GetWeaponOwner()->GetMesh()->GetAnimInstance();
		if (IsValid(AnimInstance))
		{
			GetSkeletalMesh()->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_RightHandSocketName);
			Scabbard->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_LeftHandSocketName);
			AnimInstance->Montage_Play(UnequipMontage);
		}

		// �޼տ� Į��, �����տ� īŸ���� ���������.
		// ���߿� �ִϸ��̼� �����Ǹ� ������.
	}
}

void AKatana::Attach()
{
	if (GetWeaponOwner())
	{
		ACharacter* Character = Cast<ACharacter>(GetWeaponOwner());

		// �� ���ʿ� Į��, �����տ� īŸ���� ���������.
		GetSkeletalMesh()->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_RightHandSocketName);
		Scabbard->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_UnequipSocketName);
	}
}
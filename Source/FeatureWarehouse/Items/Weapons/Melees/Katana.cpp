// Fill out your copyright notice in the Description page of Project Settings.


#include "Melees/Katana.h"
#include "AnimInstance/PlayerAnimInstance.h"
#include "Enums/TypeOfMelee.h"

#include "NiagaraComponent.h"
#include "GameFramework/Character.h"

AKatana::AKatana()
{
	ScabbardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScabbardMesh"));
	ScabbardMesh->SetCollisionProfileName(FName("NoCollision"));
	ScabbardMesh->SetupAttachment(RootComponent);

	SetMeleeType(ETypeOfMelee::Katana);
}

void AKatana::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Set SlashFX's User Parameters.
	SlashFX->SetNiagaraVariableFloat(FString("Elemental_Lifetime"), 0.0f);
	SlashFX->SetNiagaraVariableFloat(FString("Slash_Lifetime"), 1.0f);
	SlashFX->SetNiagaraVariableFloat(FString("Slash_Width"), 60.0f);
}

// 카타나를 꺼낼 때 쓰이는 함수.
void AKatana::Equip()
{
	if (GetWeaponOwner() && IsValid(MontageInfo.m_EquipMontage))
	{
		UAnimInstance* AnimInstance = GetWeaponOwner()->GetMesh()->GetAnimInstance();
		if (IsValid(AnimInstance))
		{
			// 왼손에 칼집, 칼날을 보여줘야함.
			BladeMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_LeftHandSocketName);
			ScabbardMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_LeftHandSocketName);
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
			// 몸 왼쪽에 칼집, 오른 손에 칼날.
			BladeMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_RightHandSocketName);
			ScabbardMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_UnequipSocketName);
		}
		else
		{
			// 둘 다 몸 왼쪽에 어태치.
			BladeMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_UnequipSocketName);
			ScabbardMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_UnequipSocketName);
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
			// 왼손에 칼집, 오른손에 카타나를 보여줘야함.
			BladeMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_RightHandSocketName);
			ScabbardMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_LeftHandSocketName);
			AnimInstance->Montage_Play(MontageInfo.m_UnequipMontage);
		}
	}
}

void AKatana::EquipInstantly()
{
	// 몸 왼쪽에 칼집, 오른 손에 칼날.
	BladeMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_RightHandSocketName);
	ScabbardMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_UnequipSocketName);

	OnEquipEnded(MontageInfo.m_EquipMontage, false);
}

void AKatana::UnequipInstantly()
{
	// 둘 다 몸 왼쪽에 어태치.
	BladeMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_UnequipSocketName);
	ScabbardMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_UnequipSocketName);

	OnUnequipEnded(MontageInfo.m_UnequipMontage, false);
}

void AKatana::Attach()
{
	if (GetWeaponOwner())
	{
		// 둘 다 몸 왼쪽에 어태치.
		BladeMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_UnequipSocketName);
		ScabbardMesh->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_UnequipSocketName);
	}
}

void AKatana::Detach()
{
	ScabbardMesh->AttachToComponent(BladeMesh, FAttachmentTransformRules::KeepRelativeTransform);
}

void AKatana::ActiveOverlay()
{
	BladeMesh->SetOverlayMaterial(OutlineMaterial);
	ScabbardMesh->SetOverlayMaterial(OutlineMaterial);
}

void AKatana::DeactiveOverlay()
{
	BladeMesh->SetOverlayMaterial(nullptr);
	ScabbardMesh->SetOverlayMaterial(nullptr);
}
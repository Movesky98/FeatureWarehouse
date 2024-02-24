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

// 카타나를 꺼낼 때 쓰이는 함수.
void AKatana::Equip()
{
	if (GetWeaponOwner() && IsValid(EquipMontage))
	{
		UAnimInstance* AnimInstance = GetWeaponOwner()->GetMesh()->GetAnimInstance();
		if (IsValid(AnimInstance))
		{
			// 왼손에 칼집, 칼날을 보여줘야함.
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
			// 몸 왼쪽에 칼집, 오른 손에 칼날.
			GetSkeletalMesh()->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_RightHandSocketName);
			Scabbard->AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_UnequipSocketName);
		}
		else
		{
			// 둘 다 몸 왼쪽에 어태치.
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

		// 왼손에 칼집, 오른손에 카타나를 보여줘야함.
		// 나중에 애니메이션 정리되면 만들어보자.
	}
}

void AKatana::Attach()
{
	if (GetWeaponOwner())
	{
		ACharacter* Character = Cast<ACharacter>(GetWeaponOwner());

		// 몸 왼쪽에 칼집, 오른손에 카타나를 보여줘야함.
		GetSkeletalMesh()->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_RightHandSocketName);
		Scabbard->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_UnequipSocketName);
	}
}
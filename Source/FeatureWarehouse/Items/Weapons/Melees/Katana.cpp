// Fill out your copyright notice in the Description page of Project Settings.


#include "Melees/Katana.h"

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
	if (GetWeaponOwner())
	{
		ACharacter* Character = Cast<ACharacter>(GetWeaponOwner());
		
		// �޼տ� Į��, �����տ� īŸ���� ���������.
		// ���߿� �ִϸ��̼� �����Ǹ� ������.
	}
}

void AKatana::Unequip()
{

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
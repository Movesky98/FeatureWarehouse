// Fill out your copyright notice in the Description page of Project Settings.


#include "Melees/Katana.h"

#include "GameFramework/Character.h"

AKatana::AKatana()
{
	Scabbard = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Scabbard"));
	Scabbard->SetupAttachment(RootComponent);
	Scabbard->SetCollisionProfileName(FName("NoCollision"));
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
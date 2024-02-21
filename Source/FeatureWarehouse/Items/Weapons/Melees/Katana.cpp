// Fill out your copyright notice in the Description page of Project Settings.


#include "Melees/Katana.h"

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
	if (GetWeaponOwner())
	{
		ACharacter* Character = Cast<ACharacter>(GetWeaponOwner());
		
		// 왼손에 칼집, 오른손에 카타나를 보여줘야함.
		// 나중에 애니메이션 정리되면 만들어보자.
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

		// 몸 왼쪽에 칼집, 오른손에 카타나를 보여줘야함.
		GetSkeletalMesh()->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Blade_RightHandSocketName);
		Scabbard->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Scabbard_UnequipSocketName);
	}
}
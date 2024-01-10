// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"

#include "../Characters/PlayerCharacter.h"
#include "../AnimInstance/PlayerAnimInstance.h"

#include "../Items/Weapon.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());

	if (PlayerCharacter)
	{
		UAnimInstance* AnimInstance = PlayerCharacter->GetMesh()->GetAnimInstance();

		if (AnimInstance)
		{
			PlayerAnim = Cast<UPlayerAnimInstance>(AnimInstance);
		}
	}
}

void UWeaponComponent::SaveWeaponInfo(AWeapon* NewWeapon)
{
	switch (EquipNum)
	{
	case 0:

		break;
	case 1:

		break;
	case 2:

		break;

	default:
		// Error.
		break;
	}
}

void UWeaponComponent::SetWeaponInfo(AWeapon* NewWeapon, bool IsFirst)
{
	if (IsFirst)
	{
		if (NewWeapon == nullptr) return;

		FirstWeapon = NewWeapon;
		EquipNum++;

		return;
	}
	else
	{

		return;
	}
}

void UWeaponComponent::NotifyToAnimInstance()
{

}
// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"

#include "Characters/PlayerCharacter.h"
#include "AnimInstance/PlayerAnimInstance.h"

#include "Items/Weapon.h"
#include "Items/Gun.h"
#include "Items/Melee.h"

#include "Enums/UseTypeOfWeapon.h"
#include "Enums/TypeOfWeapon.h"

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
	if (NewWeapon)
	{
		switch (EquipNum)
		{
		case 0:
			EquipNum++;
			FirstWeapon = NewWeapon;
			FirstWeapon->SetUseType(EUseTypeOfWeapon::Main);
			SetWeaponInfo(FirstWeapon);

			PlayerAnim->SetHasWeapon(true);
			NotifyToAnimInstance();
			break;
		case 1:
			EquipNum++;
			SecondWeapon = NewWeapon;
			SecondWeapon->SetUseType(EUseTypeOfWeapon::Sub);
			SetWeaponInfo(SecondWeapon);
			break;
		case 2:
			ChangeToNewWeapon(NewWeapon);
			break;
		default:
			// Error.
			break;
		}
	}
}

void UWeaponComponent::ChangeToNewWeapon(AWeapon* NewWeapon)
{
	FVector DropLocation = NewWeapon->GetActorLocation();

	NewWeapon->SetUseType(EUseTypeOfWeapon::Main);

	if (FirstWeapon->GetUseType() == EUseTypeOfWeapon::Main)
	{
		FirstWeapon->ThrowAway(DropLocation);
		FirstWeapon = NewWeapon;
		SetWeaponInfo(FirstWeapon);
	}
	else
	{
		// SecondWeapon is main weapon.
		SecondWeapon->ThrowAway(DropLocation);
		SecondWeapon = NewWeapon;
		SetWeaponInfo(SecondWeapon);
	}

	NotifyToAnimInstance();
}

void UWeaponComponent::SetWeaponInfo(AWeapon* NewWeapon)
{
	NewWeapon->TakeUp(GetOwner());

	if (PlayerCharacter)
	{
		NewWeapon->AttachToComponent(PlayerCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, NewWeapon->GetAttachSocketName());

		// if the new weapon is the first weapon, setting it as the player's main wepaon.
		(NewWeapon->GetUseType() == EUseTypeOfWeapon::Main) ? PlayerCharacter->SetMainWeapon(NewWeapon) : NewWeapon->GetSkeletalMesh()->SetVisibility(false);
	}
}

void UWeaponComponent::NotifyToAnimInstance()
{
	AWeapon* Weapon = PlayerCharacter->GetMainWeapon();

	if (!Weapon)
		return;

	ETypeOfWeapon WeaponType = Weapon->GetWeaponType();

	PlayerAnim->SetWeaponType(WeaponType);

	switch (WeaponType)
	{
	case ETypeOfWeapon::Gun:
	{
		// Check gun's type.
		AGun* Gun = Cast<AGun>(Weapon);
		if (!Gun) break;

		PlayerAnim->SetGunType(Gun->GetGunType());
	}
		break;
	case ETypeOfWeapon::Halberd:

		break;
	case ETypeOfWeapon::Melee:
	{
		AMelee* Melee = Cast<AMelee>(Weapon);
		if (!Melee) break;

		Melee->BindMontage();
	}
		break;
	case ETypeOfWeapon::Wand:
		break;
	default:
		break;
	}


}

void UWeaponComponent::SwapWeapon()
{
	if (PlayerCharacter->GetMainWeapon() == SecondWeapon)
	{
		// Swap second weapon to first weapon.
		FirstWeapon->GetSkeletalMesh()->SetVisibility(true);
		FirstWeapon->SetUseType(EUseTypeOfWeapon::Main);

		SecondWeapon->GetSkeletalMesh()->SetVisibility(false);
		SecondWeapon->SetUseType(EUseTypeOfWeapon::Sub);
	}
	else
	{
		SecondWeapon->GetSkeletalMesh()->SetVisibility(true);
		SecondWeapon->SetUseType(EUseTypeOfWeapon::Main);

		FirstWeapon->GetSkeletalMesh()->SetVisibility(false);
		FirstWeapon->SetUseType(EUseTypeOfWeapon::Sub);
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Enums/UseTypeOfWeapon.h"
#include "Enums/TypeOfWeapon.h"

#include "Characters/PlayerCharacter.h"
#include "Components/WeaponComponent.h"

#include "Components/SphereComponent.h"

AWeapon::AWeapon()
{
	UseType = EUseTypeOfWeapon::None;
}

void AWeapon::Interact(AActor* InteractActor)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(InteractActor);

	if (PlayerCharacter)
	{
		PlayerCharacter->GetWeaponComponent()->SaveWeaponInfo(this);
	}
}

void AWeapon::TakeUp(AActor* NewOwner)
{
	GetTriggerZone()->SetCollisionProfileName(FName("NoCollision"));

	SetActorLocation(FVector(0.0f, 0.0f, 0.0f));

	SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));

	WeaponOwner = NewOwner;
}

void AWeapon::ThrowAway(FVector Location)
{
	WeaponOwner = nullptr;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	GetTriggerZone()->SetCollisionProfileName(FName("Item"));

	SetActorLocation(Location);
}

void AWeapon::Attack()
{

}

bool AWeapon::CanAttack()
{
	return false;
}
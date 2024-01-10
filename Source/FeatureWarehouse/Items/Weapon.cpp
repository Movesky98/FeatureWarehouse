// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "../Enums/UseTypeOfWeapon.h"

#include "../Characters/PlayerCharacter.h"
#include "../Components/WeaponComponent.h"

#include "Components/SphereComponent.h"

void AWeapon::Interact(AActor* InteractActor)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(InteractActor);

	if (PlayerCharacter)
	{
		PlayerCharacter->GetWeaponComponent()->SaveWeaponInfo(this);
	}
}

void AWeapon::PickUp()
{
	GetTriggerZone()->SetCollisionProfileName(FName("NoCollision"));

	SetActorLocation(FVector(0.0f, 0.0f, 0.0f));
}

void AWeapon::PutDown(FVector Location)
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	GetTriggerZone()->SetCollisionProfileName(FName("Item"));

	SetActorLocation(Location);
}
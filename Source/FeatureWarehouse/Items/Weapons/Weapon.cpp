// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Enums/UseTypeOfWeapon.h"
#include "Enums/TypeOfWeapon.h"
#include "Enums/StateOfViews.h"

#include "Characters/PlayerCharacter.h"
#include "Components/WeaponComponent.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"

AWeapon::AWeapon()
{

}

void AWeapon::Interact(AActor* InteractActor)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(InteractActor);

	if (PlayerCharacter)
	{
		PlayerCharacter->GetWeaponComponent()->SaveAcquiredWeaponInfo(this);
	}
}

void AWeapon::TakeUp(ACharacter* NewOwner)
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
	SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
	SetActorScale3D(FVector(1.0f));
}

void AWeapon::Attack(EStateOfViews CurView, FVector HitLocation)
{

}

void AWeapon::StopAttack()
{

}

bool AWeapon::CanAttack()
{
	return false;
}

void AWeapon::Attach() 
{

}

void AWeapon::Equip()
{

}

void AWeapon::Unequip()
{

}
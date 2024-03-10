// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Enums/UseTypeOfWeapon.h"
#include "Enums/TypeOfWeapon.h"
#include "Enums/StateOfViews.h"

#include "Characters/WeaponWielder.h"
#include "Components/WeaponComponent.h"
#include "Components/StatComponent.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Animation/AnimMontage.h"

AWeapon::AWeapon()
{

}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (IsValid(EquipMontage) && IsValid(UnequipMontage))
	{
		bHasEquipMontage = true;
	}
}

void AWeapon::Interact(AActor* InteractActor)
{
	AWeaponWielder* InteractOwner = Cast<AWeaponWielder>(InteractActor);

	if (!IsValid(InteractOwner)) return;
	
	InteractOwner->GetWeaponComponent()->SaveAcquiredWeaponInfo(this);
	InteractOwner->GetStatComponent()->SetMontages(GetDamagedMontage, DeathMontage);
}

void AWeapon::TakeUp(ACharacter* NewOwner)
{
	GetTriggerZone()->SetCollisionProfileName(FName("NoCollision"));
	GetItemCollision()->SetCollisionProfileName(FName("NoCollision"));

	SetActorLocation(FVector(0.0f, 0.0f, 0.0f));

	SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));

	WeaponOwner = NewOwner;
}

void AWeapon::ThrowAway(FVector Location)
{
	WeaponOwner = nullptr;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Detach();

	GetItemCollision()->SetCollisionProfileName(FName("Item"));
	GetTriggerZone()->SetCollisionProfileName(FName("PlayerTrigger"));

	SetActorLocation(Location);
	SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
	SetActorScale3D(FVector(1.0f));
}

void AWeapon::Attack()
{

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
	if (IsValid(GetWeaponOwner()))
	{
		AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, AttachSocketName);
	}
}

void AWeapon::Detach()
{
	
}

void AWeapon::Equip()
{
	if (!IsValid(EquipMontage)) return;
	if (!IsValid(WeaponOwner)) return;

	UAnimInstance* AnimInstance = WeaponOwner->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(EquipMontage);
	}
}

void AWeapon::Unequip()
{
	if (!IsValid(UnequipMontage)) return;
	if (!IsValid(WeaponOwner)) return;

	UAnimInstance* AnimInstance = WeaponOwner->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(UnequipMontage);
	}
}
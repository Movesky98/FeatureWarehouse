// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Enums/UseTypeOfWeapon.h"
#include "Enums/TypeOfWeapon.h"
#include "Enums/StateOfViews.h"

#include "Characters/WeaponWielder.h"
#include "AnimInstance/WielderAnimInstance.h"

#include "Components/WeaponComponent.h"
#include "Components/StatComponent.h"
#include "Components/ItemDescriptionComponent.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
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

void AWeapon::BindMontage()
{
	if (!GetWeaponOwner()) return;

	AWeaponWielder* Wielder = Cast<AWeaponWielder>(GetWeaponOwner());
	if (!Wielder) return;

	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(Wielder->GetMesh()->GetAnimInstance());
	if (WielderAnim)
	{
		WielderAnim->OnMontageEnded.AddDynamic(this, &AWeapon::OnEquipEnded);
		WielderAnim->OnMontageEnded.AddDynamic(this, &AWeapon::OnUnequipEnded);

		UE_LOG(LogTemp, Warning, TEXT("Weapon - %s :: BindMontage is called."), *UKismetSystemLibrary::GetDisplayName(this));
	}
}

void AWeapon::UnbindMontage()
{
	if (!GetWeaponOwner()) return;

	AWeaponWielder* Wielder = Cast<AWeaponWielder>(GetWeaponOwner());
	if (!Wielder) return;

	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(Wielder->GetMesh()->GetAnimInstance());
	if (WielderAnim)
	{
		WielderAnim->OnMontageEnded.Clear();
		UE_LOG(LogTemp, Warning, TEXT("Weapon - %s :: UnbindMontage is called."), *UKismetSystemLibrary::GetDisplayName(this));
	}
}

void AWeapon::OnEquipEnded(class UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == EquipMontage)
	{
		bIsEquip = true;

		// Notify To WeaponOwner.
		AWeaponWielder* WeaponWielder = Cast<AWeaponWielder>(GetWeaponOwner());

		ensureMsgf(WeaponWielder != nullptr, TEXT("ERROR :: Weapon's Owner is not WeaponWielder."));

		WeaponWielder->OnEquipEnded();
	}
}

void AWeapon::OnUnequipEnded(class UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == UnequipMontage)
	{
		bIsEquip = false;
		UnbindMontage();

		// Notify To WeaponOwner.
		AWeaponWielder* WeaponWielder = Cast<AWeaponWielder>(GetWeaponOwner());

		ensureMsgf(WeaponWielder != nullptr, TEXT("ERROR :: Weapon's Owner is not WeaponWielder."));

		WeaponWielder->OnUnequipEnded();
	}
}

void AWeapon::Interact(AActor* InteractActor)
{
	AWeaponWielder* InteractOwner = Cast<AWeaponWielder>(InteractActor);

	if (!IsValid(InteractOwner)) return;
	TMap<FString, UAnimMontage*> DeliverMontages;

	DeliverMontages.Emplace(FString("DeathMontage"), DeathMontage);
	DeliverMontages.Emplace(FString("GetDamagedMontage"), GetDamagedMontage);
	DeliverMontages.Emplace(FString("KnockdownMontage"), KnockdownMontage);
	DeliverMontages.Emplace(FString("RetreatMontage"), RetreatMontage);

	InteractOwner->GetWeaponComponent()->SaveAcquiredWeaponInfo(this);
	InteractOwner->GetStatComponent()->SetMontages(DeliverMontages);
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

UTexture2D* AWeapon::GetWeaponImage() 
{ 
	return ItemDescriptionComponent->GetIcon(); 
}
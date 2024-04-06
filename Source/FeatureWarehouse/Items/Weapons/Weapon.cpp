// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Enums/UseTypeOfWeapon.h"
#include "Enums/TypeOfWeapon.h"
#include "Enums/StateOfViews.h"
#include "Enums/Direction.h"

#include "Characters/WeaponWielder.h"
#include "AnimInstance/WielderAnimInstance.h"

#include "Components/SphereComponent.h"
#include "Components/WeaponComponent.h"
#include "Components/StatComponent.h"
#include "Components/ItemDescriptionComponent.h"

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

}

void AWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UnbindMontage();
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
	}
}

void AWeapon::SaveDodgeMontages(TMap<EDirection, UAnimMontage*>& Montages)
{

}

void AWeapon::OnEquipEnded(class UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == MontageInfo.m_EquipMontage)
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
	if (Montage == MontageInfo.m_UnequipMontage)
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

	DeliverMontages.Emplace(FString("DeathMontage"), MontageInfo.m_DeathMontage);
	DeliverMontages.Emplace(FString("GetDamagedMontage"), MontageInfo.m_HitMontage);
	DeliverMontages.Emplace(FString("KnockdownMontage"), nullptr);
	DeliverMontages.Emplace(FString("RetreatMontage"), MontageInfo.m_RetreatMontage);

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
	if (!IsValid(MontageInfo.m_EquipMontage)) return;
	if (!IsValid(WeaponOwner)) return;

	UAnimInstance* AnimInstance = WeaponOwner->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(MontageInfo.m_EquipMontage);
	}
}

void AWeapon::Unequip()
{
	if (!IsValid(MontageInfo.m_UnequipMontage)) return;
	if (!IsValid(WeaponOwner)) return;

	UAnimInstance* AnimInstance = WeaponOwner->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(MontageInfo.m_UnequipMontage);
	}
}

UTexture2D* AWeapon::GetWeaponImage() 
{ 
	return ItemDescriptionComponent->GetIcon(); 
}

void AWeapon::InvokeExpandStamina(float Amount) 
{
	if (OnExpandStamina.IsBound())
	{
		OnExpandStamina.Execute(Amount);
	}
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), EquipSound, GetActorLocation(), 0.5f);
	}
}

void AWeapon::PlayUnequipSound()
{
	if (UnequipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), UnequipSound, GetActorLocation(), 0.5f);
	}
}
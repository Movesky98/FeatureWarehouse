// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWielder.h"

#include "Enums/ActionState.h"
#include "Enums/MovementState.h"
#include "Weapon.h"

#include "Components/StatComponent.h"
#include "Components/WeaponComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
// 나중에 수정 필요.
#include "GamePlay/FW_GameInstance.h"
#include "Widgets/PlayerMenu.h"

AWeaponWielder::AWeaponWielder()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SKM_Manny(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny"));
	if (SKM_Manny.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SKM_Manny.Object);
	}

	MovementState = EMovementState::EMS_Idle;
}

void AWeaponWielder::BeginPlay()
{
	Super::BeginPlay();
	

}

void AWeaponWielder::PlayMontage(UAnimMontage* Montage)
{
	UAnimInstance* CharacterAnim = GetMesh()->GetAnimInstance();

	if (CharacterAnim)
	{
		CharacterAnim->Montage_Play(Montage);
	}
}

void AWeaponWielder::Attack()
{

}

void AWeaponWielder::StopAttack()
{

}

void AWeaponWielder::HeavyAttack()
{

}

void AWeaponWielder::EquipFirstWeapon()
{

}

void AWeaponWielder::EquipSecondWeapon()
{

}

void AWeaponWielder::Unequip()
{

}

void AWeaponWielder::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	ACharacter::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (PrevMovementMode != EMovementMode::MOVE_Falling) return;

	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
	{
		// Jump Ended.
		if (IsValid(EquipWeapon) && EquipWeapon->IsAttacking())
			WeaponComponent->JumpAttackLanding();

		MovementState = EMovementState::EMS_Idle;
	}
}

void AWeaponWielder::GetDamaged(float Damage)
{
	StatComponent->GetDamaged(Damage);

	/*if (IsDamaged)
	{
		UpdateHealth();
	}*/
}

void AWeaponWielder::EquipEnded()
{

}

void AWeaponWielder::UnequipEnded()
{

}
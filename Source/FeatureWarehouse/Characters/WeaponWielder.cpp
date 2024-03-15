// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWielder.h"

#include "Enums/ActionState.h"
#include "Enums/MovementState.h"
#include "Weapon.h"

#include "Components/StatComponent.h"
#include "Components/WeaponComponent.h"
#include "Components/CapsuleComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
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

void AWeaponWielder::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	this->OnTakePointDamage.AddDynamic(this, &AWeaponWielder::OnReceivePointDamageEvent);
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

void AWeaponWielder::OnReceivePointDamageEvent(AActor* DamagedActor, float Damage, AController* InstigatedBy, 
	FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, 
	FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	/* 
	* To Do List
	* 1. Decrease HP
	* 2. Show Blood effect.
	* 3. Play hit sound.
	*/
	if (bIsDead) return;

	APawn* ControlledPawn = InstigatedBy->GetPawn();

	StatComponent->DecreaseHP(Damage);

	FRotator ImpactRotation = UKismetMathLibrary::MakeRotFromZ(ShotFromDirection);
	StatComponent->ShowBloodEffect(HitLocation, ImpactRotation);
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

void AWeaponWielder::Die()
{
	GetCapsuleComponent()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
}

void AWeaponWielder::EquipEnded()
{

}

void AWeaponWielder::UnequipEnded()
{

}

bool AWeaponWielder::CheckTakeAction(EActionState SpecificAction)
{
	// 특정 액션을 취하고 있지 않는 경우, 실행 가능.
	if (ActionState == EActionState::EAS_Idle)
	{
		ActionState = SpecificAction;
		return true;
	}

	// 특정 액션을 취하는데 일치할 경우 true 아니면 false.
	return  ActionState == SpecificAction ? true : false;
}
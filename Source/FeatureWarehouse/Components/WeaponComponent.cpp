// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"

#include "Characters/WeaponWielder.h"
#include "Characters/PlayerCharacter.h"

#include "AnimInstance/PlayerAnimInstance.h"
#include "AnimInstance/WielderAnimInstance.h"

#include "Weapon.h"
#include "Guns/Gun.h"
#include "Melees/Melee.h"

#include "Enums/UseTypeOfWeapon.h"
#include "Enums/TypeOfWeapon.h"
#include "Enums/MovementState.h"
#include "Enums/ActionState.h"


// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...

	EquipState = EEquipState::None;
}

// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	WeaponWielder = Cast<AWeaponWielder>(GetOwner());

	if (WeaponWielder)
	{
		WielderAnim = Cast<UWielderAnimInstance>(WeaponWielder->GetMesh()->GetAnimInstance());
	}
}

void UWeaponComponent::EquipMainWeapon()
{
	if (!IsValid(MainWeapon)) return;
	if (WeaponWielder->CurWeapon() == MainWeapon) return;

	EquipState = EEquipState::MainWeapon;

	WeaponWielder->SetWeapon(MainWeapon);
	
	UE_LOG(LogTemp, Warning, TEXT("WeaponComponent :: Called EquipMainWeapon."));

	NotifyToAnimInstance();
}

void UWeaponComponent::EquipSubWeapon()
{
	if (!IsValid(SubWeapon) || WeaponWielder->CurWeapon() == SubWeapon) return;

	EquipState = EEquipState::SubWeapon;

	WeaponWielder->SetWeapon(SubWeapon);

	UE_LOG(LogTemp, Warning, TEXT("WeaponComponent :: Called EquipSubWeapon."));

	NotifyToAnimInstance();
}

void UWeaponComponent::EquipOtherWeapon()
{
	// ���� ��� �ִ� ���Ⱑ �����̳Ŀ� ���� �ݴ� ���⸦ ������.
	EquipState == EEquipState::MainWeapon ? EquipSubWeapon()
		: EquipMainWeapon();
}

void UWeaponComponent::Unequip()
{
	if (!IsValid(WeaponWielder->CurWeapon()) && EquipState == EEquipState::None) return;

	WeaponWielder->CurWeapon()->Unequip();
	WeaponWielder->SetWeapon(nullptr);
	NotifyToAnimInstance();
	EquipState = EEquipState::None;
}

void UWeaponComponent::SaveAcquiredWeaponInfo(AWeapon* NewWeapon)
{
	if (EquipNum < 0) return;

	switch (EquipNum)
	{
	case 0:
		// ó�� ȹ���� ������ �� == ���� ����� ���.
		EquipNum++;
		SaveMainWeaponInfo(NewWeapon);
		break;
	case 1:
		// �ι�° ȹ���� ������ ��
		EquipNum++;
		SaveSingleWeaponInfo(NewWeapon);
		return;
		break;
	case 2:
		// �̹� �� ���� ���⸦ ������ ���� �� (���� ��ü)
		SaveMultipleWeaponInfo(NewWeapon);
		break;
	default:
		break;
	}

	NotifyToAnimInstance();
}

void UWeaponComponent::SaveSingleWeaponInfo(AWeapon* Weapon)
{
	// ���߿� �κ��丮�� �����ϰ� �Ǹ�, ��Ʋ �׶���ó�� ����, ���� ���⸦ ������� �ٲ� �� �ֵ��� �����ϰ���.
	// ������ ���⸦ ȹ���ϸ� �ι�° ����� ������ ��������.

	SaveSubWeaponInfo(Weapon);
}

void UWeaponComponent::SaveMultipleWeaponInfo(AWeapon* Weapon)
{
	switch (EquipState)
	{
	case EEquipState::None:
	case EEquipState::MainWeapon:
		SaveMainWeaponInfo(Weapon);
		break;
	case EEquipState::SubWeapon:
		SaveSubWeaponInfo(Weapon);
		break;
	default:
		break;
	}
}

void UWeaponComponent::SaveMainWeaponInfo(AWeapon* Weapon)
{
	if (IsValid(MainWeapon))
	{
		// ���� ���Ⱑ �̹� �ִ� ��� (���� ��ü)
		FVector DropLocation = Weapon->GetActorLocation();
		MainWeapon->ThrowAway(DropLocation);
	}

	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	Weapon->TakeUp(CharacterOwner);

	Weapon->Attach();

	MainWeapon = Weapon;
	if (EquipState == EEquipState::MainWeapon)
	{
		WeaponWielder->SetWeapon(MainWeapon);
	}

	NotifyHasWeaponToAnim();
}

void UWeaponComponent::SaveSubWeaponInfo(AWeapon* Weapon)
{
	if (IsValid(SubWeapon))
	{
		// ���� ���Ⱑ �̹� �ִ� ��� (���� ��ü)
		FVector DropLocation = Weapon->GetActorLocation();
		SubWeapon->ThrowAway(DropLocation);
	}

	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	Weapon->TakeUp(CharacterOwner);

	// ���⸦ ĳ���Ϳ� ����ġ
	Weapon->Attach();

	SubWeapon = Weapon;

	if (EquipState == EEquipState::SubWeapon)
	{
		WeaponWielder->SetWeapon(SubWeapon);
	}

	NotifyHasWeaponToAnim();
}

void UWeaponComponent::NotifyToAnimInstance()
{
	if (!IsValid(WielderAnim)) return;

	WeaponWielder->IsA<APlayerCharacter>() ? 
		UpdateWeaponInfoToPlayerAnimInstance() :
		UpdateWeaponInfoToWielderAnimInstance();

	// ���� ��� (�ִϸ��̼��� ����. ����� �ڵ� �����ϱ�)
	if (!WeaponWielder->CurWeapon()) return;

	if (WeaponWielder->CurWeapon()->GetWeaponType() == ETypeOfWeapon::Gun)
	{
		 WeaponWielder->SetActionState(EActionState::EAS_Idle);
		 WeaponWielder->OnEquipEnded();
	}
}

void UWeaponComponent::UpdateWeaponInfoToPlayerAnimInstance()
{
	UPlayerAnimInstance* PlayerAnim = Cast<UPlayerAnimInstance>(WielderAnim);
	if (!IsValid(PlayerAnim)) return;

	if (!IsValid(WeaponWielder->CurWeapon()))
	{
		PlayerAnim->SetHasWeapon(false);
		return;
	}

	AWeapon* Weapon = WeaponWielder->CurWeapon();

	Weapon->Equip();
	Weapon->BindMontage();

	ETypeOfWeapon WeaponType = Weapon->GetWeaponType();
	PlayerAnim->SetWeaponType(WeaponType);
	PlayerAnim->SetHasWeapon(true);

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
	case ETypeOfWeapon::Melee:
	{
		AMelee* Melee = Cast<AMelee>(Weapon);
		if (!Melee) break;

		PlayerAnim->SetMeleeType(Melee->GetMeleeType());
	}
	break;
	case ETypeOfWeapon::Wand:
		break;
	default:
		break;
	}
}

void UWeaponComponent::UpdateWeaponInfoToWielderAnimInstance()
{
	if (!IsValid(WeaponWielder->CurWeapon())) return;
	
	AWeapon* Weapon = WeaponWielder->CurWeapon();

	Weapon->Equip();
	Weapon->BindMontage();
	
	ETypeOfWeapon WeaponType = Weapon->GetWeaponType();
	switch (WeaponType)
	{
	case ETypeOfWeapon::Gun:
	{
		// Check gun's type.

	}
	break;
	case ETypeOfWeapon::Melee:
	{

	}
	break;
	case ETypeOfWeapon::Wand:
		break;
	default:
		break;
	}
}

void UWeaponComponent::NotifyHasWeaponToAnim()
{
	UPlayerAnimInstance* PlayerAnim = Cast<UPlayerAnimInstance>(WielderAnim);

	if (IsValid(PlayerAnim))
	{
		PlayerAnim->SetHasWeapon(true);
	}
}

// ���� ���⸦ ��� ���������� �ϴٰ� ���� ����� �� ����Ǵ� �Լ�.
void UWeaponComponent::JumpAttackLanding()
{
	if (!IsValid(WeaponWielder->CurWeapon())) return;

	AMelee* Melee = Cast<AMelee>(WeaponWielder->CurWeapon());
	if (IsValid(Melee))
	{
		Melee->JumpAttackLanding();
	}
}

// ���⸦ ������ �� ����ϴ� �Լ�
void UWeaponComponent::Swap()
{
	WeaponWielder->SetActionState(EActionState::EAS_Swapping);
	WeaponWielder->CurWeapon()->Unequip();
}
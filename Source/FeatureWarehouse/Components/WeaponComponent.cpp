// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"

#include "Characters/PlayerCharacter.h"
#include "AnimInstance/PlayerAnimInstance.h"

#include "Weapon.h"
#include "Guns/Gun.h"
#include "Melees/Melee.h"

#include "Enums/UseTypeOfWeapon.h"
#include "Enums/TypeOfWeapon.h"

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

	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());

	if (PlayerCharacter)
	{
		UPlayerAnimInstance* AnimInstance = Cast<UPlayerAnimInstance>(PlayerCharacter->GetMesh()->GetAnimInstance());

		if (AnimInstance)
		{
			PlayerAnim = AnimInstance;
			PlayerAnim->OnUnequipEnd.BindUFunction(this, FName("OnUnequipEnd"));
		}
	}
}

void UWeaponComponent::OnUnequipEnd()
{
	if (!PlayerCharacter) return;
	PlayerCharacter->SetActionState(EActionState::EAS_Idle);

	// Unequip이 끝났으므로, 무기 교체.
	EquipState == EEquipState::SubWeapon ? EquipMainWeapon() : EquipSubWeapon();
}

void UWeaponComponent::EquipMainWeapon()
{
	if (!IsValid(MainWeapon) || PlayerCharacter->GetMainWeapon() == MainWeapon) return;

	EquipState = EEquipState::MainWeapon;

	PlayerCharacter->SetMainWeapon(MainWeapon);
	
	NotifyToAnimInstance();
}

void UWeaponComponent::EquipSubWeapon()
{
	if (!IsValid(SubWeapon) || PlayerCharacter->GetMainWeapon() == SubWeapon) return;

	EquipState = EEquipState::SubWeapon;

	PlayerCharacter->SetMainWeapon(SubWeapon);

	NotifyToAnimInstance();
}

void UWeaponComponent::SaveAcquiredWeaponInfo(AWeapon* NewWeapon)
{
	if (EquipNum < 0) return;

	switch (EquipNum)
	{
	case 0:
		// 처음 획득한 무기일 때 == 메인 무기로 등록.
		EquipNum++;
		EquipState = EEquipState::MainWeapon;
		NotifyHasWeaponToAnim();
		SaveMainWeaponInfo(NewWeapon);
		break;
	case 1:
		// 두번째 획득한 무기일 때
		EquipNum++;
		SaveSingleWeaponInfo(NewWeapon);
		return;
		break;
	case 2:
		// 이미 두 개의 무기를 가지고 있을 때 (무기 교체)
		SaveMultipleWeaponInfo(NewWeapon);
		break;
	default:
		break;
	}

	NotifyToAnimInstance();
}

void UWeaponComponent::SaveSingleWeaponInfo(AWeapon* Weapon)
{
	// 나중에 인벤토리를 구현하게 되면, 배틀 그라운드처럼 메인, 서브 무기를 마음대로 바꿀 수 있도록 구현하겠음.
	// 지금은 무기를 획득하면 두번째 무기로 들어가도록 만들어놓음.

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
		// 메인 무기가 이미 있는 경우 (무기 교체)
		FVector DropLocation = Weapon->GetActorLocation();
		MainWeapon->ThrowAway(DropLocation);
	}

	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	Weapon->TakeUp(CharacterOwner);

	Weapon->Attach();

	MainWeapon = Weapon;
	if (EquipState == EEquipState::MainWeapon)
	{
		PlayerCharacter->SetMainWeapon(MainWeapon);
	}
}

void UWeaponComponent::SaveSubWeaponInfo(AWeapon* Weapon)
{
	if (IsValid(SubWeapon))
	{
		// 메인 무기가 이미 있는 경우 (무기 교체)
		FVector DropLocation = Weapon->GetActorLocation();
		SubWeapon->ThrowAway(DropLocation);
	}

	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	Weapon->TakeUp(CharacterOwner);

	// 무기를 캐릭터에 어태치
	Weapon->Attach();

	SubWeapon = Weapon;

	if (EquipState == EEquipState::SubWeapon)
	{
		PlayerCharacter->SetMainWeapon(SubWeapon);
	}
}

void UWeaponComponent::NotifyToAnimInstance()
{
	if (!IsValid(PlayerCharacter->GetMainWeapon())) return;

	AWeapon* Weapon = PlayerCharacter->GetMainWeapon();
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
	case ETypeOfWeapon::Melee:
	{
		AMelee* Melee = Cast<AMelee>(Weapon);
		if (!Melee) break;

		PlayerAnim->SetMeleeType(Melee->GetMeleeType());
		Melee->BindMontage();
	}
		break;
	case ETypeOfWeapon::Wand:
		break;
	default:
		break;
	}

	PlayerCharacter->GetMainWeapon()->Equip();
}

void UWeaponComponent::NotifyHasWeaponToAnim()
{
	if (IsValid(PlayerAnim))
	{
		PlayerAnim->SetHasWeapon(true);
	}
}

// 근접 무기를 들고 점프공격을 하다가 땅에 닿았을 때 실행되는 함수.
void UWeaponComponent::JumpAttackLanding()
{
	if (!IsValid(PlayerCharacter->GetMainWeapon())) return;

	AMelee* Melee = Cast<AMelee>(PlayerCharacter->GetMainWeapon());
	if (IsValid(Melee))
	{
		Melee->JumpAttackLanding();
	}
}

// 무기를 스왑할 때 사용하는 함수
void UWeaponComponent::Swap()
{
	PlayerCharacter->SetActionState(EActionState::EAS_Swapping);
	PlayerCharacter->GetMainWeapon()->Unequip();
}
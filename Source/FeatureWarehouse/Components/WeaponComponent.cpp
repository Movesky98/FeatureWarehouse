// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"

#include "Characters/WielderBase.h"
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
	EquipState = EEquipState::None;
}

// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	WielderBase = Cast<AWielderBase>(GetOwner());

	if (WielderBase)
	{
		WielderAnim = Cast<UWielderAnimInstance>(WielderBase->GetMesh()->GetAnimInstance());
	}
}

void UWeaponComponent::EquipMainWeapon()
{
	if (!IsValid(MainWeapon)) return;
	if (WielderBase->CurWeapon() == MainWeapon) return;

	EquipState = EEquipState::MainWeapon;

	WielderBase->SetWeapon(MainWeapon);
	
	UE_LOG(LogTemp, Warning, TEXT("WeaponComponent :: Called EquipMainWeapon."));

	NotifyToAnimInstance();
}

void UWeaponComponent::EquipSubWeapon()
{
	if (!IsValid(SubWeapon) || WielderBase->CurWeapon() == SubWeapon) return;

	EquipState = EEquipState::SubWeapon;

	WielderBase->SetWeapon(SubWeapon);

	UE_LOG(LogTemp, Warning, TEXT("WeaponComponent :: Called EquipSubWeapon."));

	NotifyToAnimInstance();
}

void UWeaponComponent::EquipOtherWeapon()
{
	// 현재 들고 있는 무기가 무엇이냐에 따라 반대 무기를 장착함.
	EquipState == EEquipState::MainWeapon ? EquipSubWeapon() : EquipMainWeapon();
}

void UWeaponComponent::Unequip()
{
	if (!IsValid(WielderBase->CurWeapon()) && EquipState == EEquipState::None) return;

	WielderBase->CurWeapon()->Unequip();
	WielderBase->SetWeapon(nullptr);
	NotifyToAnimInstance();
	EquipState = EEquipState::None;
}

void UWeaponComponent::SaveAcquiredWeaponInfo(AWeapon* NewWeapon)
{
	if (EquipNum < 0) return;

	switch (EquipNum)
	{
	case 0:
		// 처음 획득한 무기일 때 == 메인 무기로 등록.
		EquipNum++;
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
		WielderBase->SetWeapon(MainWeapon);
	}

	NotifyHasWeaponToAnim();
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
		WielderBase->SetWeapon(SubWeapon);
	}

	NotifyHasWeaponToAnim();
}

void UWeaponComponent::NotifyToAnimInstance()
{
	if (!IsValid(WielderAnim)) return;

	WielderBase->IsA<APlayerCharacter>() ? 
		UpdateWeaponInfoToPlayerAnimInstance() :
		UpdateWeaponInfoToWielderAnimInstance();

	// 총의 경우 (애니메이션이 없음. 생기면 코드 수정하기)
	if (!WielderBase->CurWeapon()) return;

	if (WielderBase->CurWeapon()->GetWeaponType() == ETypeOfWeapon::Gun)
	{
		 WielderBase->SetActionState(EActionState::EAS_Idle);
		 WielderBase->OnEquipEnded();
	}
}

void UWeaponComponent::UpdateWeaponInfoToPlayerAnimInstance()
{
	UPlayerAnimInstance* PlayerAnim = Cast<UPlayerAnimInstance>(WielderAnim);
	if (!IsValid(PlayerAnim)) return;

	if (!IsValid(WielderBase->CurWeapon()))
	{
		PlayerAnim->SetHasWeapon(false);
		return;
	}

	AWeapon* Weapon = WielderBase->CurWeapon();

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
	if (!IsValid(WielderBase->CurWeapon())) return;
	
	AWeapon* Weapon = WielderBase->CurWeapon();

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

// 근접 무기를 들고 점프공격을 하다가 땅에 닿았을 때 실행되는 함수.
void UWeaponComponent::JumpAttackLanding()
{
	if (!IsValid(WielderBase->CurWeapon())) return;

	AMelee* Melee = Cast<AMelee>(WielderBase->CurWeapon());
	if (IsValid(Melee))
	{
		Melee->JumpAttackLanding();
	}
}

// 무기를 스왑할 때 사용하는 함수
void UWeaponComponent::Swap()
{
	WielderBase->SetActionState(EActionState::EAS_Swapping);
	WielderBase->CurWeapon()->Unequip();
}

void UWeaponComponent::RemoveOwnerWeapon()
{
	WielderBase->SetWeapon(nullptr);

	if (MainWeapon)
	{
		MainWeapon->UnbindMontage();
		MainWeapon->SetWeaponOwner(nullptr);
		MainWeapon->Destroy();
	}

	if (SubWeapon)
	{
		SubWeapon->UnbindMontage();
		SubWeapon->SetWeaponOwner(nullptr);
		SubWeapon->Destroy();
	}
}
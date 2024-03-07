// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponWielder.generated.h"

enum class EMovementState :uint8;
enum class EActionState :uint8;

class AWeapon;
class UWeaponComponent;
class UStatComponent;

/* 
* WeaponComponent를 사용할 수 있는 캐릭터 클래스
* 
* 기존에 구현했던 플레이어만 사용하던 WeaponComponent를 AI도 함께 사용하고자 하여 PlayerCharacter로부터 분리하였음.
*/
UCLASS()
class FEATUREWAREHOUSE_API AWeaponWielder : public ACharacter
{
	GENERATED_BODY()

public:
	AWeaponWielder();

	void PlayMontage(UAnimMontage* Montage);

	virtual void GetDamaged(float Damage);

	virtual void EquipEnded();

	virtual void UnequipEnded();

protected:
	virtual void BeginPlay() override;

	// Functions
	virtual void Attack();

	virtual void StopAttack();

	virtual void HeavyAttack();

	UFUNCTION(BlueprintCallable)
	virtual void EquipFirstWeapon();

	UFUNCTION(BlueprintCallable)
	virtual void EquipSecondWeapon();

	UFUNCTION(BlueprintCallable)
	virtual void Unequip();

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	// Variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	UWeaponComponent* WeaponComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	UStatComponent* StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeapon* EquipWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bHasWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|State")
	EMovementState MovementState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|State")
	EActionState ActionState;

#pragma region GetterSetter
public:
	// Equip Weapon
	FORCEINLINE AWeapon* CurWeapon() { return EquipWeapon; }
	FORCEINLINE void SetWeapon(AWeapon* Weapon) { EquipWeapon = Weapon; }

	// Components
	FORCEINLINE UWeaponComponent* GetWeaponComponent() { return WeaponComponent; }
	FORCEINLINE UStatComponent* GetStatComponent() { return StatComponent; }

	// States
	FORCEINLINE EMovementState CurMovementState() { return MovementState; }
	FORCEINLINE void SetMovementState(EMovementState State) { MovementState = State; }
	FORCEINLINE EActionState CurActionState() { return ActionState; }
	FORCEINLINE void SetActionState(EActionState State) { ActionState = State; }
#pragma endregion
};

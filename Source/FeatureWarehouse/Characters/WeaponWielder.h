// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponWielder.generated.h"

enum class EMovementState :uint8;
enum class EActionState :uint8;
enum class EDirection :uint8;

class AWeapon;
class UWeaponComponent;
class UStatComponent;
class UStatBarComponent;

/* 
* WeaponComponent�� ����� �� �ִ� ĳ���� Ŭ����
* 
* ������ �����ߴ� �÷��̾ ����ϴ� WeaponComponent�� AI�� �Բ� ����ϰ��� �Ͽ� PlayerCharacter�κ��� �и��Ͽ���.
*/
UCLASS()
class FEATUREWAREHOUSE_API AWeaponWielder : public ACharacter
{
	GENERATED_BODY()

public:
	AWeaponWielder();

	void PlayMontage(UAnimMontage* Montage);

	virtual void OnEquipEnded();

	virtual void OnUnequipEnded();

	virtual void Die();

protected:
	virtual void PostInitializeComponents() override;
	
	virtual void BeginPlay() override;

	// Functions
	virtual void Attack();

	/* Receive Point Damage Function */
	UFUNCTION() virtual void OnReceivePointDamageEvent(AActor* DamagedActor, float Damage, AController* InstigatedBy, 
		FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName,
		FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser);
	
	virtual void StopAttack();

	virtual void HeavyAttack();

	UFUNCTION(BlueprintCallable)
	virtual void EquipFirstWeapon();

	UFUNCTION(BlueprintCallable)
	virtual void EquipSecondWeapon();

	UFUNCTION(BlueprintCallable)
	virtual void Unequip();

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	/* �׼��� ���� �� �ִ��� üũ�ϴ� �Լ�. */
	bool CheckTakeAction(EActionState SpecificAction);

	// Variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	UWeaponComponent* WeaponComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	UStatComponent* StatComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	class UNiagaraComponent* AfterImageComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeapon* EquipWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bHasWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|State")
	EMovementState MovementState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|State")
	EActionState ActionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State")
	bool bIsDead;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage")
	TMap<EDirection, UAnimMontage*> DodgeMontages;

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

	FORCEINLINE bool IsDead() { return bIsDead; }
	FORCEINLINE void SetIsDead(bool IsDead) { bIsDead = IsDead; }
#pragma endregion
};

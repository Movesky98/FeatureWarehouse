// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

enum class EUseTypeOfWeapon :uint8;
enum class ETypeOfWeapon :uint8;
enum class EStateOfViews :uint8;
class UAnimMontage;

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API AWeapon : public AItem 
{
	GENERATED_BODY()
	
public:
	AWeapon();

	void Interact(AActor* InteractActor) override;

	UFUNCTION(BlueprintCallable)
	void TakeUp(ACharacter* NewOwner);

	UFUNCTION(BlueprintCallable)
	void ThrowAway(FVector Location);
	
	UFUNCTION(BlueprintCallable)
	virtual void Attack();

	virtual void Attack(EStateOfViews CurView, FVector HitLocation = FVector(0.0f));

	virtual void StopAttack();

	virtual void Attach();

	virtual void Detach();

	virtual void Equip();

	virtual void Unequip();

protected:
	virtual bool CanAttack();

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintImplementableEvent)
	void ShakePlayerCamera();

	UFUNCTION(BlueprintImplementableEvent)
	void StopShake();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* UnequipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage|Hit", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* GetDamagedMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage|Death", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage|Dodge", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RetreatMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage|Knockdown", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KnockdownMontage;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	ACharacter* WeaponOwner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	FName AttachSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	ETypeOfWeapon WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool bHasAttachmentInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool bHasEquipMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	bool bIsAttacking;

#pragma region GetterSetter
public:
	FORCEINLINE ACharacter* GetWeaponOwner() { return WeaponOwner; }
	FORCEINLINE void SetWeaponOwner(ACharacter* NewOwner) { WeaponOwner = NewOwner; }

	FORCEINLINE FName GetAttachSocketName() { return AttachSocketName; }

	FORCEINLINE ETypeOfWeapon GetWeaponType() { return WeaponType; }

	FORCEINLINE bool IsAttacking() { return bIsAttacking; }
	FORCEINLINE void SetIsAttacking(bool IsAttacking) { bIsAttacking = IsAttacking; }

	FORCEINLINE bool HasEquipMontage() { return bHasEquipMontage; }
	FORCEINLINE bool HasAttachmentInfo() { return bHasAttachmentInfo; }
#pragma endregion
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Structs/MeleeAttackInfo.h"
#include "Structs/MeleeMontageInfo.h"
#include "Weapon.generated.h"

DECLARE_DELEGATE_OneParam(FOnExpandStaminaDelegate, float);

enum class EUseTypeOfWeapon :uint8;
enum class ETypeOfWeapon :uint8;
enum class EStateOfViews :uint8;
enum class EDirection :uint8;

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

	FOnExpandStaminaDelegate OnExpandStamina;

	void Interact(AActor* InteractActor) override;

	UFUNCTION(BlueprintCallable)
	virtual void TakeUp(ACharacter* NewOwner);

	UFUNCTION(BlueprintCallable)
	virtual void ThrowAway(FVector Location);
	
	virtual void BindMontage();

	virtual void UnbindMontage();

	UFUNCTION(BlueprintCallable)
	virtual void Attack();

	virtual void SaveDodgeMontages(TMap<EDirection, UAnimMontage*>& Montages);

	virtual void Attack(EStateOfViews CurView, FVector HitLocation = FVector(0.0f));

	virtual void StopAttack();

	virtual void Attach();

	virtual void Detach();

	virtual void Equip();

	virtual void Unequip();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnEquipEnded(class UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnUnequipEnded(class UAnimMontage* Montage, bool bInterrupted);

	void PlayEquipSound();

	void PlayUnequipSound();

	void InvokeExpandStamina(float Amount);

	virtual bool CanAttack();

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintImplementableEvent)
	void ShakePlayerCamera();

	UFUNCTION(BlueprintImplementableEvent)
	void StopShake();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage|Info", meta = (AllowPrivateAccess = "true"))
	FMeleeMontageInfo MontageInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack|Info", meta = (AllowPrivateAccess = "true"))
	TArray<FMeleeAttackInfo> LightAttackInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack|Info", meta = (AllowPrivateAccess = "true"))
	TArray<FMeleeAttackInfo> HeavyAttackInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack|Info", meta = (AllowPrivateAccess = "true"))
	FMeleeAttackInfo JumpAttackInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack|Info", meta = (AllowPrivateAccess = "true"))
	FMeleeAttackInfo SprintAttackInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsEquip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	FName WeaponName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bHasEquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage|Sound", meta = (AllowPrivateAccess = "true"))
	class USoundCue* EquipSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage|Sound", meta = (AllowPrivateAccess = "true"))
	class USoundCue* UnequipSound;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	ACharacter* WeaponOwner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	FName AttachSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	ETypeOfWeapon WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool bHasAttachmentInfo;

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

	UTexture2D* GetWeaponImage();
#pragma endregion
};

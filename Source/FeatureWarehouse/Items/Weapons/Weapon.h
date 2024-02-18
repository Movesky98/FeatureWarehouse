// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

enum class EUseTypeOfWeapon :uint8;
enum class ETypeOfWeapon :uint8;
enum class EStateOfViews :uint8;
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
	void TakeUp(AActor* NewOwner);

	UFUNCTION(BlueprintCallable)
	void ThrowAway(FVector Location);

	virtual void Attack(EStateOfViews CurView, FVector HitLocation = FVector(0.0f));

	virtual void StopAttack();

	virtual void Attach();

	virtual void Equip();

	virtual void Unequip();

protected:
	virtual bool CanAttack();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	AActor* WeaponOwner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	FName AttachSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	ETypeOfWeapon WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool bHasAttachmentInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	bool bIsAttacking;

#pragma region GetterSetter
public:
	FORCEINLINE AActor* GetWeaponOwner() { return WeaponOwner; }
	FORCEINLINE void SetWeaponOwner(AActor* NewOwner) { WeaponOwner = NewOwner; }

	FORCEINLINE FName GetAttachSocketName() { return AttachSocketName; }

	FORCEINLINE ETypeOfWeapon GetWeaponType() { return WeaponType; }

	FORCEINLINE bool IsAttacking() { return bIsAttacking; }
	FORCEINLINE void SetIsAttacking(bool IsAttacking) { bIsAttacking = IsAttacking; }

	FORCEINLINE bool HasAttachmentInfo() { return bHasAttachmentInfo; }
#pragma endregion
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WielderAnimInstance.generated.h"

DECLARE_DELEGATE(FOnHoldMeleeWeaponDelegate);
DECLARE_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_DELEGATE(FOnPlaySlashSoundDelegate);
DECLARE_DELEGATE(FOnPlayEquipSoundDelegate);
DECLARE_DELEGATE(FOnPlayUnequipSoundDelegate);

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API UWielderAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	FOnNextAttackCheckDelegate OnNextAttackCheck;

	FOnHoldMeleeWeaponDelegate OnHoldMeleeWeapon;

	FOnPlaySlashSoundDelegate OnPlaySlashSound;

	FOnPlayEquipSoundDelegate OnPlayEquipSound;

	FOnPlayUnequipSoundDelegate OnPlayUnequipSound;

protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void AnimNotify_NextAttackCheck();

	UFUNCTION()
	void AnimNotify_HoldMeleeWeapon();

	UFUNCTION()
	void AnimNotify_PlaySlashSound();

	UFUNCTION()
	void AnimNotify_PlayEquipSound();

	UFUNCTION()
	void AnimNotify_PlayUnequipSound();


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	float Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	float Direction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	bool bIsFalling;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	bool bIsCrouch;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	bool bShouldMove;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	bool bIsEquip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	bool bIsDead;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class UCharacterMovementComponent* MovementComponent;
public:
	FORCEINLINE void SetEquipWeapon(bool IsEquip) { bIsEquip = IsEquip; }
	FORCEINLINE float GetDirection() { return Direction; }

	FORCEINLINE void SetIsDead(bool IsDead) { bIsDead = IsDead; }
};

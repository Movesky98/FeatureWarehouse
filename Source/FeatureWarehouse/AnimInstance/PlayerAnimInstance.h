// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

enum class ETypeOfWeapon :uint8;
enum class ETypeOfGun :uint8;
enum class ETypeOfMelee :uint8;

DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPlayerAnimInstance();

	FOnNextAttackCheckDelegate OnNextAttackCheck;

protected:
	void NativeInitializeAnimation() override;

	void NativeUpdateAnimation(float DeltaSeconds) override;
	
private:
	// Functions
	UFUNCTION()
	void AnimNotify_NextAttackCheck();

	// Variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	float Direction;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	float Pitch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	bool bIsFalling;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	bool bIsCrouch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	bool bShouldMove;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	ETypeOfWeapon WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	ETypeOfGun GunType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	ETypeOfMelee MeleeType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	bool bHasWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	bool bIsEquip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	bool bAttemptToggleEquip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Player, meta = (AllowPrivateAccess = "true"))
	class APlayerCharacter* Player;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Player, meta = (AllowPrivateAccess = "true"))
	class UCharacterMovementComponent* MovementComponent;

#pragma region GetterSetter
public:
	FORCEINLINE void SetHasWeapon(bool HasWeapon) { bHasWeapon = HasWeapon; }
	FORCEINLINE void SetIsEquip(bool IsEquip) { bIsEquip = IsEquip; }
	FORCEINLINE void AttemptToggleEquip(bool IsAttempt) { bAttemptToggleEquip = IsAttempt; }

	FORCEINLINE void SetWeaponType(ETypeOfWeapon Type) { WeaponType = Type; }
	FORCEINLINE void SetMeleeType(ETypeOfMelee Type) { MeleeType = Type; }
	FORCEINLINE void SetGunType(ETypeOfGun Type) { GunType = Type; }

#pragma endregion
};

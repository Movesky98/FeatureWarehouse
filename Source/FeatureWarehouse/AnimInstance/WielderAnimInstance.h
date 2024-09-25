// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WielderAnimInstance.generated.h"

DECLARE_DELEGATE(FOnHoldMeleeWeaponDelegate);
DECLARE_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_DELEGATE(FOnHitEndDelegate);
DECLARE_DELEGATE(FOnRetreatEndDelegate);

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

	FOnHitEndDelegate OnHitEnd;

	FOnRetreatEndDelegate OnRetreatEnd;

	/** Reaction Montages */
	void SetReactionMontages(TMap<FString, UAnimMontage*> AnimMontages);

	void PlayDeathMontage();

	void PlayHitMontage();

	void PlayRetreatMontage();

	void PlayGetGroggyHitMontage(bool IsDead);

protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void OnDeath(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnHitEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnRetreatEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnGetGroggyHitEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage|Reaction")
	UAnimMontage* DeathMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage|Reaction")
	UAnimMontage* RetreatMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage|Reaction")
	UAnimMontage* HitMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage|Reaction")
	UAnimMontage* GroggyHitReactionMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage|Reaction")
	UAnimMontage* GroggyHitDeathMontage;

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

	UFUNCTION()
	void AnimNotify_GroggyAttackPoint();

	UFUNCTION()
	void AnimNotify_KnockdownStart();

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

#pragma region Execution
public:
	UFUNCTION()
	void CalculateExeuctionTime();

protected:
	FTimerHandle ExecutionTimerHandle;

	float ExecuteElapsedTime;

	float ExecutionDuration;

#pragma endregion

};

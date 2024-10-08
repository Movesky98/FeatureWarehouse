// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WielderAnimInstance.generated.h"

DECLARE_DELEGATE(FOnHoldMeleeWeaponDelegate);
DECLARE_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_DELEGATE(FOnHitEndDelegate);
DECLARE_DELEGATE(FOnRetreatEndDelegate);

DECLARE_DELEGATE(FOnGroggyAttackPointImpactDelegate);

DECLARE_DELEGATE(FOnPlaySlashSoundDelegate);
DECLARE_DELEGATE(FOnPlayEquipSoundDelegate);
DECLARE_DELEGATE(FOnPlayUnequipSoundDelegate);

enum class EPoseType :uint8;

UENUM(BlueprintType)
enum class EMontageType :uint8
{
	EMT_NONE UMETA(DisplayName = "NONE"),
	EMT_Death UMETA(DisplayName = "Death"),
	EMT_GetHit UMETA(DisplayName = "GetHit"),
	EMT_Retreat UMETA(DisplayName = "Retreat"),
	EMT_GroggyHitReaction UMETA(DisplayName = "GroggyHitReaction"),
	EMT_GroggyHitDeath UMETA(DisplayName = "GroggyHitDeath"),
	EMT_GetUpFromFront UMETA(DisplayName = "GetUpFromFront"),
	EMT_GetUpFromBack UMETA(DisplayName = "GetUpFromBack")
};

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

	FOnGroggyAttackPointImpactDelegate OnGroggyAttackPointImpact;

	/* Reaction Montages */
	void SetReactionMontages(TMap<FString, UAnimMontage*> AnimMontages);

	void PlayReactionMontage(EMontageType MontageType);

	UFUNCTION(BlueprintSetter)
	void SetPoseType(EPoseType Type);

	UFUNCTION(BlueprintGetter)
	EPoseType GetPoseType();

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

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage|Reaction")
	UAnimMontage* GetUpFromFrontMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage|Reaction")
	UAnimMontage* GetUpFromBackMontage;

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

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "State")
	EPoseType PoseType;

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

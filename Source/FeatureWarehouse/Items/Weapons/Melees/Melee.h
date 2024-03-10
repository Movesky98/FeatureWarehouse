// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Melee.generated.h"

enum class EStateOfViews :uint8;
enum class ETypeOfMelee :uint8;

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API AMelee : public AWeapon
{
	GENERATED_BODY()
	
public:
	AMelee();

	virtual void BindMontage();

	void JumpAttackLanding();

protected:
	virtual void BeginPlay() override;

	void Attack() override;

	void Attack(EStateOfViews CurView, FVector HitLocation = FVector(0.0f)) override;

	virtual bool CanAttack() override;

	virtual class UAnimMontage* FindAppropriateAttackAnimation();

	UFUNCTION()
	virtual	void HoldMeleeWeapon();

	UFUNCTION()
	void EquipEnded();

	UFUNCTION()
	void UnequipEnded();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool bIsEquip;

private:
	// Functions
	UFUNCTION()
	void OnAttackEnded(class UAnimMontage* Montage, bool bInterrupted);

	bool IsAttackMontage(UAnimMontage* Montage);

	UFUNCTION()
	void OnNextAttackChecked();

	UFUNCTION(BlueprintCallable)
	void StartAttackTrace();

	UFUNCTION(BlueprintCallable)
	void StopAttackTrace();

	UFUNCTION()
	void AttackTrace();

	void DrawAttackLineTrace(const FVector& LineStart, const FVector& LineEnd, bool IsInterpolating);

	// Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool CanCombo;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State", meta = (AllowPrivateAccess = "true"))
	ETypeOfMelee MeleeType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	TArray<class UAnimMontage*> AttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* SprintAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* JumpAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* JumpAttackLandMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HeavyAttackMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	int MontageIndex;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> IgnoreActor;

	FTimerHandle AttackTraceTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Interpolating", meta = (AllowPrivateAccess = "true"))
	float InterpolateDistance;

	float BladeLength;

	FVector PreBladeVector;

	FVector PreStart;

	FVector PreMidpoint;

public:
	FORCEINLINE ETypeOfMelee GetMeleeType() { return MeleeType; }
	FORCEINLINE void SetMeleeType(ETypeOfMelee Type) { MeleeType = Type; }
};

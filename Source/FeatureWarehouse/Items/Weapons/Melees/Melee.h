// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Melee.generated.h"

enum class EStateOfViews :uint8;
enum class ETypeOfMelee :uint8;

class UDataTable;

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API AMelee : public AWeapon
{
	GENERATED_BODY()
	
public:
	AMelee();

	void BindMontage() override;

	void UnbindMontage() override;

	void JumpAttackLanding();

protected:
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	void Attack() override;

	void Attack(EStateOfViews CurView, FVector HitLocation = FVector(0.0f)) override;

	virtual bool CanAttack() override;

	virtual class UAnimMontage* FindAppropriateAttackAnimationWithDamage(float& Damage, int Index);

	UFUNCTION()
	virtual	void HoldMeleeWeapon();

	UFUNCTION(BlueprintCallable)
	void StartAttackTrace();

	UFUNCTION(BlueprintCallable)
	void StopAttackTrace();

	UFUNCTION()
	virtual void AttackTrace();

	virtual void DrawAttackLineTrace(const FVector& LineStart, const FVector& LineEnd);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Interpolating")
	float InterpolateDistance;

	float BladeLength;

	FVector PreBladeVector;

	FVector PreStart;

	FVector PreMidpoint;
	
	FTimerHandle AttackTraceTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Montage")
	int MontageIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage")
	TArray<AActor*> IgnoreActor;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	float Damage;
private:
	// Functions
	UFUNCTION()
	void OnAttackEnded(class UAnimMontage* Montage, bool bInterrupted);

	bool IsAttackMontage(UAnimMontage* Montage);

	UFUNCTION()
	void OnNextAttackChecked();

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

	UPROPERTY()
	UDataTable* MeleeTable;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	TArray<float> LightAttackDamage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	TArray<float> HeavyAttackDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	float JumpAttackDamage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	float SprintAttackDamage;

public:
	FORCEINLINE ETypeOfMelee GetMeleeType() { return MeleeType; }
	FORCEINLINE void SetMeleeType(ETypeOfMelee Type) { MeleeType = Type; }
};

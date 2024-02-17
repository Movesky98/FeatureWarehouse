// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Melee.generated.h"

enum class EStateOfViews :uint8;

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API AMelee : public AWeapon
{
	GENERATED_BODY()
	
public:
	AMelee();

	UFUNCTION(BlueprintCallable)
	void BindMontage();

protected:
	virtual void BeginPlay() override;

	virtual void Attack(EStateOfViews CurView, FVector HitLocation = FVector(0.0f)) override;

	virtual bool CanAttack() override;

	UFUNCTION()
	void OnBladeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBladeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	// Functions
	UFUNCTION()
	void OnAttackEnded(class UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnNextAttackChecked();

	UFUNCTION(BlueprintCallable)
	void StartAttackTrace();

	UFUNCTION(BlueprintCallable)
	void StopAttackTrace();

	UFUNCTION()
	void AttackTrace();

	// Variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	bool CanCombo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Montage, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* AttackMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Montage, meta = (AllowPrivateAccess = "true"))
	int MontageIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BladeCollision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> IgnoreActor;

	FTimerHandle AttackTraceTimer;
};

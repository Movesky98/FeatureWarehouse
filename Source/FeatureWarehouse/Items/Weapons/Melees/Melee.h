// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Structs/MeleeAttackInfo.h"
#include "Structs/MeleeMontageInfo.h"
#include "Melee.generated.h"

enum class EStateOfViews :uint8;
enum class ETypeOfMelee :uint8;

struct FMeleeMontageInfo;
struct FMeleeAttackInfo;

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

	UFUNCTION(BlueprintCallable)
	void ActivateSlashFX();

	UFUNCTION(BlueprintCallable)
	void DeactivateSlashFX();

protected:
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void ActiveOverlay() override;

	void DeactiveOverlay() override;

	void SaveDodgeMontages(TMap<EDirection, UAnimMontage*>& Montages) override;

	void Attack() override;

	void Attack(EStateOfViews CurView, FVector HitLocation = FVector(0.0f)) override;

	virtual bool CanAttack() override;

	virtual class UAnimMontage* FindAppropriateAttackAnimationWithParam(float& Change_Damage, int Index, float& Stamina, class USoundCue*& Sound);

	UFUNCTION()
	virtual	void HoldMeleeWeapon();

	UFUNCTION() 
	void PlaySlashSound();

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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	float StaminaCost;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	class USoundCue* SlashSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	class USoundCue* HitSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	bool bIsHitSomething;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BladeMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* SlashFX;

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

	UPROPERTY()
	UDataTable* MeleeInfoTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> AttackMontages;

public:
	UFUNCTION(BlueprintGetter)
	class UStaticMeshComponent* GetBladeMesh() { return BladeMesh; }

	FORCEINLINE ETypeOfMelee GetMeleeType() { return MeleeType; }
	FORCEINLINE void SetMeleeType(ETypeOfMelee Type) { MeleeType = Type; }
};

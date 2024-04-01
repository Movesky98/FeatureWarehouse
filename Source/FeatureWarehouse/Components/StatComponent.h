// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetDamagedDelegate, bool);
DECLARE_DELEGATE(FOnExhaustedStaminaDelegate);

class UParticleSystem;

UENUM(BlueprintType)
enum class EDamagableType :uint8
{
	EDT_INVINCIBLE UMETA(DisplayName = "INVINCIBLE"),		// ����
	EDT_VULNERABLE UMETA(DisplayName = "VULNERABLE"),		// �������� ���� �� �ִ�
	EDT_EVADING UMETA(DisplayName = "EVADING")				// ȸ�� ����
};

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FEATUREWAREHOUSE_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStatComponent();

	UFUNCTION(BlueprintCallable)
	void DecreaseHP(float Damage);

	void DecreaseStamina(float Amount);

	void ShowBloodEffect(FVector Location, FRotator Rotation);

	void StartRecoveryStamina(float WaitTime = 0);

	void StopRecoveryStamina();

	void SetMontages(TMap<FString, UAnimMontage*> AnimMontages);

	void SetDamagableType(EDamagableType Type);

	FOnGetDamagedDelegate OnGetDamaged;

	FOnExhaustedStaminaDelegate OnExhaustedStamina;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void RecoveryStamina();

	UFUNCTION()
	void CalculateStaminaWaitTime();

	/** ���¹̳� ��� �ð� */
	float StaminaElapsedTime = 0.0f;

	/** Stamina ȸ���� ���۵Ǵµ� �ɸ��� �ð� */
	float RecoveryStaminaWaitTime = 5.0f;

	/** Stamina ȸ�� ������ ���� �ð� ����� ���۵Ǿ�����. */
	bool bIsStartRecoveryStaminaWaitTimer;

	/** ȸ�� ���¹̳� Ÿ�̸� */
	FTimerHandle RecoveryStaminaTimer;

	/** ���¹̳��� ���Ǿ����� */
	bool bIsExhaustedStamina;

	/** ���¹̳� ȸ���� ���۵Ǿ����� */
	bool bIsStartRecoveryStamina;

	void PlayMontage(UAnimMontage* PlayMontage);

	UFUNCTION()
	void OnDeathEnded(UAnimMontage* Montage, bool bInterrupted);

	void PlayDeathMontage();

	/* �ǰ� �ִϸ��̼��� ���� ���� ����Ǵ� �Լ� */
	UFUNCTION() void OnGetDamagedEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float MaxHP;			// �ִ� ü��

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float CurrentHP;		// ���� ü��

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float MaxMP;			// �ִ� ����

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float CurrentMP;		// ���� ����

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float CurrentStamina;	// ���� ���¹̳�

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float MaxStamina;		// �ִ� ���¹̳�

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float DamageAccumulation;	// ������ ������ (�ѹ��� ������ ���ذ� >= CurrentHP's 30% �� ���, ȸ��)

	FTimerHandle ResetDamageAccumulationTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float StunGauge;		// ���� ������

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float StunGaugeLimit;	// �ִ� ���ϰ����� (<= StunGage�� ���, �׷α� ����)

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool bIsDamagable;		// �������� �޴� ��ü����

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State", meta = (AllowPrivateAccess = "true"))
	EDamagableType DamagableType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BloodParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* GetDamagedMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* DeathMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* RetreatMontage;

#pragma region GetterSetter
public:
	FORCEINLINE float GetMaxHP() { return MaxHP; }
	FORCEINLINE void SetMaxHP(float NewMaxHP) { MaxHP = NewMaxHP; }

	FORCEINLINE float GetCurrentHP() { return CurrentHP; }
	FORCEINLINE void SetCurrentHP(float NewCurrentHP) { CurrentHP = NewCurrentHP; }

	FORCEINLINE float GetMaxMP() { return MaxMP; }
	FORCEINLINE void SetMaxMP(float NewMaxMP) { MaxMP = NewMaxMP; }

	FORCEINLINE float GetCurrentMP() { return CurrentMP; }
	FORCEINLINE void SetCurrentMP(float NewCurrentMP) { CurrentMP = NewCurrentMP; }

	FORCEINLINE float GetMaxStamina() { return MaxStamina; }
	FORCEINLINE void SetMaxStamina(float NewMaxStamina) { MaxStamina = NewMaxStamina; }

	FORCEINLINE float GetCurrentStamina() { return CurrentStamina; }
	FORCEINLINE void SetCurrentStamina(float NewCurrentStamina) { CurrentStamina = NewCurrentStamina; }

	FORCEINLINE class UAnimMontage* GetBackstepMontage() { return RetreatMontage; }

	FORCEINLINE EDamagableType GetDamagableType() { return DamagableType; }
	
#pragma endregion
};

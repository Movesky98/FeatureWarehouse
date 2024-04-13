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
	EDT_INVINCIBLE UMETA(DisplayName = "INVINCIBLE"),		// 무적 상태
	EDT_VULNERABLE UMETA(DisplayName = "VULNERABLE"),		// 데미지를 받을 수 있는 상태
	EDT_EVADING UMETA(DisplayName = "EVADING")				// 회피 중인 상태
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

	bool CheckDeathStatus();

	bool CheckShouldEvade();

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

	/** 스태미나 경과 시간 */
	float StaminaElapsedTime = 0.0f;

	/** Stamina 회복이 시작되는데 걸리는 시간 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	float RecoveryStaminaWaitTime = 3.0f;

	/** Stamina 회복 시작을 위한 시간 계산이 시작되었는지. */
	bool bIsStartRecoveryStaminaWaitTimer;

	/** 회복 스태미나 타이머 */
	FTimerHandle RecoveryStaminaTimer;

	/** 스태미나가 고갈되었는지 */
	bool bIsExhaustedStamina;

	/** 스태미나 회복이 시작되었는지 */
	bool bIsStartRecoveryStamina;

	void PlayMontage(UAnimMontage* PlayMontage);

	UFUNCTION()
	void OnDeathEnded(UAnimMontage* Montage, bool bInterrupted);

	void PlayDeathMontage();

	/* 피격 애니메이션이 끝난 이후 실행되는 함수 */
	UFUNCTION() void OnGetDamagedEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float MaxHP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float CurrentHP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float MaxMP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float CurrentMP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float CurrentStamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float MaxStamina;

	/** 데미지 누적량 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float DamageAccumulation;

	/** 회피 임계치. 전체 체력의 얼마만큼 데미지를 입어야 회피를 할 지 결정하기 위한 변수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat", meta = (UIMin = "0", UIMax = "1", AllowPrivateAccess ="true"))
	float EvadeThreshold;

	FTimerHandle ResetDamageAccumulationTimer;

	/** 스턴 게이지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float StunGauge;

	/** 최대 스턴게이지 (<= StunGage일 경우, 그로기 상태) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float StunGaugeLimit;

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

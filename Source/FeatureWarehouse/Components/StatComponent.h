// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetDamagedDelegate, bool);

class UParticleSystem;

UENUM(BlueprintType)
enum class EDamagableType :uint8
{
	EDT_INVINCIBLE UMETA(DisplayName = "INVINCIBLE"),		// 무적
	EDT_VULNERABLE UMETA(DisplayName = "VULNERABLE"),		// 데미지를 받을 수 있는
	EDT_EVADING UMETA(DisplayName = "EVADING")				// 회피 중인
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

	bool DecreaseStamina(float Amount);

	void ShowBloodEffect(FVector Location, FRotator Rotation);

	void SetMontages(TMap<FString, UAnimMontage*> AnimMontages);

	void SetDamagableType(EDamagableType Type);

	FOnGetDamagedDelegate OnGetDamaged;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void PlayMontage(UAnimMontage* PlayMontage);

	UFUNCTION()
	void OnDeathEnded(UAnimMontage* Montage, bool bInterrupted);

	void PlayDeathMontage();

	/* 피격 애니메이션이 끝난 이후 실행되는 함수 */
	UFUNCTION() void OnGetDamagedEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float MaxHP;			// 최대 체력

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float CurrentHP;		// 현재 체력

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float MaxMP;			// 최대 마나

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float CurrentMP;		// 현제 마나

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float CurrentStamina;	// 현재 스태미나

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float MaxStamina;		// 최대 스태미나

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float DamageAccumulation;	// 데미지 누적량 (한번에 들어오는 피해가 >= CurrentHP's 30% 일 경우, 회피)

	FTimerHandle ResetDamageAccumulationTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float StunGauge;		// 스턴 게이지

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float StunGaugeLimit;	// 최대 스턴게이지 (<= StunGage일 경우, 그로기 상태)

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool bIsDamagable;		// 데미지를 받는 객체인지

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

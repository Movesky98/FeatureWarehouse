// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"

DECLARE_DELEGATE(FOnRetreatFromEnemyDelegate);

class UParticleSystem;

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FEATUREWAREHOUSE_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatComponent();
	
	UFUNCTION(BlueprintCallable)
	void DecreaseHP(float Damage);

	void ShowBloodEffect(FVector Location, FRotator Rotation);

	void SetMontages(TMap<FString, UAnimMontage*> AnimMontages);

	FOnRetreatFromEnemyDelegate OnRetreatFromEnemy;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void PlayMontage(UAnimMontage* PlayMontage);

	UFUNCTION()
	void OnDeathEnded();

	/* 데미지를 받은 애니메이션이 끝난 이후 실행되는 함수 */
	UFUNCTION() void OnGetDamagedEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	float MaxHP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	float CurrentHP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	float MaxMP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	float CurrentMP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	bool bIsDamagable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
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

	FORCEINLINE class UAnimMontage* GetBackstepMontage() { return RetreatMontage; }
#pragma endregion
};

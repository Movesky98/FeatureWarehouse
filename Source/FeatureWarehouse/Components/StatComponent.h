// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"

class UParticleSystem;

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FEATUREWAREHOUSE_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatComponent();
	
	UFUNCTION(BlueprintCallable)
	bool GetDamaged(float Damage);

	void ShowBloodEffect(FVector Location, FRotator Rotation);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void PlayMontage(UAnimMontage* PlayMontage);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* GetDamagedMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* DeathMontage;

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

	FORCEINLINE void SetDamagable() { bIsDamagable = true; }
	
#pragma endregion
};

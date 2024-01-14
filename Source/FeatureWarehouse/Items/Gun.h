// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Gun.generated.h"

enum class ETypeOfGun :uint8;
enum class EFireMode :uint8;

/* Struct for Weapon's Information */
USTRUCT(Atomic, BlueprintType)
struct FGunInfo {
	GENERATED_BODY()

public:
	FGunInfo() : CurrentRounds(0), TotalRounds(0), MagazineSize(0)
	{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentRounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TotalRounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MagazineSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AProjectile> ProjectileClass;

	// Particle
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNiagaraSystem* NS_MuzzleFlash;

	UPROPERTY(EditAnywhere, BLueprintReadWrite)
	class UNiagaraSystem* NS_HiveShot;

	// Sounds :: FireSound, ReloadSound.

};

UCLASS()
class FEATUREWAREHOUSE_API AGun : public AWeapon
{
	GENERATED_BODY()
public:
	AGun();

	void StartFire();

	void StopFire();

protected:
	virtual void Attack() override;

	UFUNCTION()
	void Fire();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* MuzzleArrow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* HiveShotArrow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	ETypeOfGun GunType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	EFireMode FireMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Fire, meta = (AllowPrivateAccess = "true"))
	FGunInfo Info;

	FTimerHandle FireTimerHandle;

#pragma region GetterSetter
public:
	FORCEINLINE ETypeOfGun GetGunType() { return GunType; }
	FORCEINLINE void SetGunType(ETypeOfGun Type) { GunType = Type; }

#pragma endregion
};

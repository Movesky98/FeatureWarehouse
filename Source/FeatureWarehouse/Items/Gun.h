// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Gun.generated.h"

enum class ETypeOfGun :uint8;

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API AGun : public AWeapon
{
	GENERATED_BODY()
public:
	AGun();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	ETypeOfGun GunType;

#pragma region GetterSetter
public:
	FORCEINLINE ETypeOfGun GetGunType() { return GunType; }
	FORCEINLINE void SetGunType(ETypeOfGun Type) { GunType = Type; }

#pragma endregion
};

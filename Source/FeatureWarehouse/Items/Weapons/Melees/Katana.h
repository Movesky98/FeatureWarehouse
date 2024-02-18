// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Melees/Melee.h"
#include "Katana.generated.h"

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API AKatana : public AMelee
{
	GENERATED_BODY()

public:
	AKatana();

	void Equip() override;

	void Unequip() override;

protected:
	void Attach() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Scabbard;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Socket|Unequip", meta = (AllowPrivateAccess = "true"))
	FName Blade_UnequipSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Socket|Unequip", meta = (AllowPrivateAccess = "true"))
	FName Scabbard_UnequipSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Socket|Equip", meta = (AllowPrivateAccess = "true"))
	FName Blade_RightHandSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Socket|Equip", meta = (AllowPrivateAccess = "true"))
	FName Blade_LeftHandSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Socket|Equip", meta = (AllowPrivateAccess = "true"))
	FName Scabbard_LeftHandSocketName;
};

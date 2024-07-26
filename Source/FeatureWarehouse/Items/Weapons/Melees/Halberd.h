// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Melee.h"
#include "Halberd.generated.h"

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API AHalberd : public AMelee
{
	GENERATED_BODY()
public:
	AHalberd();

protected:
	virtual void PostInitializeComponents() override;

	void Equip() override;

	void Unequip() override;
	
	void Attach() override;

	void HoldMeleeWeapon() override;
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attach|Socket", meta = (AllowPrivateAccess = "true"))
	FName EquipSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attach|Socket", meta = (AllowPrivateAccess = "true"))
	FName UnequipSocketName;
};

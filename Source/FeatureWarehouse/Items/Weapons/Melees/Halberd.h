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

	/* 장착 애니메이션 없이 바로 무기를 장착하는 함수 */
	void EquipInstantly() override;

	/* 해제 애니메이션 없이 바로 무기를 해제하는 함수 */
	void UnequipInstantly() override;
	
	void Attach() override;

	void HoldMeleeWeapon() override;
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attach|Socket", meta = (AllowPrivateAccess = "true"))
	FName EquipSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attach|Socket", meta = (AllowPrivateAccess = "true"))
	FName UnequipSocketName;
};

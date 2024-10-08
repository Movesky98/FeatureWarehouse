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

	/* 장착 애니메이션 없이 바로 무기를 장착하는 함수 */
	void EquipInstantly() override;

	/* 해제 애니메이션 없이 바로 무기를 해제하는 함수 */
	void UnequipInstantly() override;

protected:
	virtual void PostInitializeComponents() override;

	void Attach() override;

	void Detach() override;

	void HoldMeleeWeapon() override;

	void ActiveOverlay() override;

	void DeactiveOverlay() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess ="true"))
	UStaticMeshComponent* ScabbardMesh;

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

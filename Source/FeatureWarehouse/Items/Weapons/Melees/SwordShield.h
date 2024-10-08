// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Melee.h"
#include "SwordShield.generated.h"

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API ASwordShield : public AMelee
{
	GENERATED_BODY()
public:
	ASwordShield();

protected:
	virtual void PostInitializeComponents() override;

	/* 장착 애니메이션 없이 바로 무기를 장착하는 함수 */
	void EquipInstantly() override;

	/* 해제 애니메이션 없이 바로 무기를 해제하는 함수 */
	void UnequipInstantly() override;

	void AttackTrace() override;

	void DrawAttackTrace(const FVector& LineStart, const FVector& LineEnd, bool IsShield);

	void TakeUp(ACharacter* NewOwner) override;

	void ThrowAway(FVector Location) override;

	void Attach() override;

	void Detach() override;

	void HoldMeleeWeapon() override;

	void ActiveOverlay() override;

	void DeactiveOverlay() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ShieldMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Socket", meta = (AllowPrivateAccess = "true"))
	FName Name_SwordEquip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Socket", meta = (AllowPrivateAccess = "true"))
	FName Name_SwordUnequip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Socket", meta = (AllowPrivateAccess = "true"))
	FName Name_ShieldEquip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Socket", meta = (AllowPrivateAccess = "true"))
	FName Name_ShieldUnequip;
};

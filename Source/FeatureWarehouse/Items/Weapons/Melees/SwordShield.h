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

	/* ���� �ִϸ��̼� ���� �ٷ� ���⸦ �����ϴ� �Լ� */
	void EquipInstantly() override;

	/* ���� �ִϸ��̼� ���� �ٷ� ���⸦ �����ϴ� �Լ� */
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

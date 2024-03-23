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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Shield;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Socket", meta = (AllowPrivateAccess = "true"))
	FName Name_SwordEquip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Socket", meta = (AllowPrivateAccess = "true"))
	FName Name_SwordUnequip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Socket", meta = (AllowPrivateAccess = "true"))
	FName Name_ShieldEquip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Socket", meta = (AllowPrivateAccess = "true"))
	FName Name_ShieldUnequip;
};

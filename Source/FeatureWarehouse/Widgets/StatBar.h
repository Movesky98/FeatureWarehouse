// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatBar.generated.h"

class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API UStatBar : public UUserWidget
{
	GENERATED_BODY()
public:
	void Init(class AWielder* NewOwner);

	void UpdateHPBar(float BarPercent, float Damage);

	void CleanDamageAccumulation();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	float DamageAccumulation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Owner")
	class AWielder* OwningWielder;

private:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HP_Bar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CharacterNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageAmountText;
};

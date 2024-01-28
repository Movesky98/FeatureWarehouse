// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menu.h"
#include "PlayerMenu.generated.h"

class UImage;
class UProgressBar;
class UTextBlock;

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API UPlayerMenu : public UMenu
{
	GENERATED_BODY()
public:
	UPlayerMenu(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void SetUp() override;

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerHealth(float CurHealth, float MaxHealth);

protected:
	virtual bool Initialize();

private:
	UPROPERTY(meta = (BindWidget))
	UImage* HP_BarImage;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HP_ProgressBar;

	UPROPERTY(meta = (BindWidget))
	UImage* Armor_BarImage;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* Armor_ProgressBar;

	UPROPERTY(meta = (BindWidget))
	UImage* Minimap;

	UPROPERTY(meta = (BindWidget))
	UImage* Crosshair;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurRoundsTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalRoundsTextBlock;
};

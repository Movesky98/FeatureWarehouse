// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerMenu.generated.h"

class UImage;
class UProgressBar;
class UTextBlock;

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API UPlayerMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UPlayerMenu(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void SetUp();

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

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menu.h"
#include "PlayerMenu.generated.h"

class UImage;
class UProgressBar;
class UTextBlock;

UENUM(BlueprintType)
enum class ETypeOfPlayerUI : uint8 
{
	ETP_ShootingGame UMETA(DisplayName = "ShootingGame"),
	ETP_RPGGame UMETA(DisplayName = "RPGGame")
};

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

	void SetRPGPanel();

	void SetShootingPanel();

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerHealth(float CurHealth, float MaxHealth);
	
	void SetStatBarSize();

protected:
	virtual bool Initialize();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	ETypeOfPlayerUI UIType;

#pragma region ShootingGame_UI
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
	UImage* Crosshair;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurRoundsTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalRoundsTextBlock;
#pragma endregion

#pragma region RPGGame_UI
private:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* RPG_HP_Bar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* RPG_MP_Bar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* RPG_Stamina_Bar;
#pragma endregion

};

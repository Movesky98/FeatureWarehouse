// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menu.h"
#include "PlayerMenu.generated.h"

class UImage;
class UProgressBar;
class UTextBlock;
class UWidgetSwitcher;
class UCanvasPanel;

UENUM(BlueprintType)
enum class ETypeOfPlayerUI : uint8 
{
	ETP_ShootingUI = 0 UMETA(DisplayName = "ShootingGame"),
	ETP_RPGUI UMETA(DisplayName = "RPGGame")
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

	UFUNCTION(BlueprintCallable)
	void SetRPGPanel();

	UFUNCTION(BlueprintCallable)
	void SetShootingPanel();

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerHealth(float CurHealth, float MaxHealth);
	
	void SetStatBarSize();

	void ChangeWeaponImage(UTexture2D* Icon);

protected:
	virtual bool Initialize();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	ETypeOfPlayerUI UIType;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* UISwitcher;

#pragma region ShootingGame_UI
private:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* ShootingGamePanel;

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

	UPROPERTY(meta = (BindWidget))
	UImage* Shooting_WeaponImage;
#pragma endregion

#pragma region RPGGame_UI
private:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* RPGGamePanel;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* RPG_HP_Bar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* RPG_MP_Bar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* RPG_Stamina_Bar;

	UPROPERTY(meta = (BindWidget))
	UImage* RPG_WeaponImage;
#pragma endregion

};

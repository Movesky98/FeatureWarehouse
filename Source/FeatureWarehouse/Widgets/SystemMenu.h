// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menu.h"
#include "SystemMenu.generated.h"

class UButton;
class UCanvasPanel;
/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API USystemMenu : public UMenu
{
	GENERATED_BODY()

public:
	USystemMenu(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void BackToDefaultPanel();

	void TearDown() override;

protected:
	virtual bool Initialize();

private:
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* UISwitcher;

#pragma region DefaultPanel
public:
	UFUNCTION()
	void ViewChangePerspectivePanel();

	UFUNCTION()
	void GameExit();

private:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* DefaultPanel;

	UPROPERTY(meta = (BindWidget))
	UButton* ChangePerspectiveButton;

	UPROPERTY(meta = (BindWidget))
	UButton* GameExitButton;
#pragma endregion

#pragma region ChangePerspectivePanel
public:
	UFUNCTION()
	void SetFPP();

	UFUNCTION()
	void SetTPP();

	UFUNCTION()
	void SetTDP();

private:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* ChangePerspectivePanel;

	UPROPERTY(meta = (BindWidget))
	UButton* FPPButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* TPPButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* TDPButton;
	
	/* Change Perspective's back button. */
	UPROPERTY(meta = (BindWidget))
	UButton* CP_BackButton;

#pragma endregion

};

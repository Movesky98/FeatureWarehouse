// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menu.h"
#include "GameEndMenu.generated.h"

class UWidgetSwitcher;
class UCanvasPanel;
class UTextBlock;
class UButton;
class UImage;

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API UGameEndMenu : public UMenu
{
	GENERATED_BODY()
public:
	UGameEndMenu(const FObjectInitializer& ObjectInitializer);

	void LoadDeadPanel();

	void LoadGameEndPanel(int EnemyKillNum, float Time, float Score);

	UFUNCTION(BlueprintImplementableEvent)
	void LoadMainMenu();

protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* DeadPanel;

	UPROPERTY(meta = (BindWidget))
	UImage* DeadImage;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* GameEndPanel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EnemyKillText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TimeTakenToClearText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalScoreText;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitMainMenuButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	class USoundCue* DeadSound;
};

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

	void SaveGameOverInfo(int p_EnemyKill, float p_Time, float p_Score);

	void LoadDeadPanel();

	void LoadGameEndPanel();

	UFUNCTION(BlueprintImplementableEvent)
	void LoadMainMenu();

protected:
	virtual bool Initialize() override;

	bool bIsClearTheGame;

	FTimerHandle LoadPanelTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameOver|Info")
	int EnemyKills;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameOver|Info")
	float GameDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameOver|Info")
	float TotalScore;

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
	UTextBlock* GameOverText;

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

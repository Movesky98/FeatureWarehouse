// Fill out your copyright notice in the Description page of Project Settings.

#include "GameEndMenu.h"

#include "Sound/SoundCue.h"

#include "Components/WidgetSwitcher.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"

#include "Kismet/GameplayStatics.h"

UGameEndMenu::UGameEndMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<USoundCue> SC_YOUDIED(TEXT("/Game/Project/Sounds/YouDied_Cue"));
	if (SC_YOUDIED.Succeeded())
	{
		DeadSound = SC_YOUDIED.Object;
	}
}

bool UGameEndMenu::Initialize()
{
	bool IsSuccess = Super::Initialize();
	if (!IsSuccess) return false;

	ExitMainMenuButton->OnClicked.AddDynamic(this, &UGameEndMenu::LoadMainMenu);

	return true;
}

void UGameEndMenu::LoadDeadPanel()
{
	WidgetSwitcher->SetActiveWidget(DeadPanel);

	// 1. Play Dead Sound.
	// 2. Show GameEnd Panel when dead sound is completed.

	UGameplayStatics::PlaySound2D(GetWorld(), DeadSound);

	GetWorld()->GetTimerManager().SetTimer(LoadPanelTimerHandle, this, &UGameEndMenu::LoadGameEndPanel, DeadSound->Duration, false);
}

void UGameEndMenu::LoadGameEndPanel()
{
	WidgetSwitcher->SetActiveWidget(GameEndPanel);

	FString GameOverMessage = bIsClearTheGame ? FString("You cleared the Game. Congratulations!") : FString("GAME OVER... It's okay. You'll do better next time.");
	FString EnemyKillString = FString("Enemy Kill : ") + FString::FromInt(EnemyKills);
	FString TimeTakenToClearString = FString("Time Taken To Clear : ") + FString::SanitizeFloat(GameDuration);
	FString TotalScoreString = FString("Total Score : ") + FString::SanitizeFloat(TotalScore);

	GameOverText->SetText(FText::FromString(GameOverMessage));
	EnemyKillText->SetText(FText::FromString(EnemyKillString));
	TimeTakenToClearText->SetText(FText::FromString(TimeTakenToClearString));
	TotalScoreText->SetText(FText::FromString(TotalScoreString));
}

void UGameEndMenu::SaveGameOverInfo(int EnemyKill, float Time, float Score)
{
	EnemyKills = EnemyKill;
	GameDuration = Time;
	TotalScore = Score;
}
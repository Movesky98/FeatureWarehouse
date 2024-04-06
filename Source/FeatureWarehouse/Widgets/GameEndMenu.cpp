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
}

void UGameEndMenu::LoadGameEndPanel(int EnemyKillNum, float Time, float Score)
{
	WidgetSwitcher->SetActiveWidget(GameEndPanel);

	FString EnemyKillString = FString("Enemy Kill : ") + FString::FromInt(EnemyKillNum);
	FString TimeTakenToClearString = FString("Time Taken To Clear : ") + FString::SanitizeFloat(Time);
	FString TotalScoreString = FString("Total Score : ") + FString::SanitizeFloat(Score);

	EnemyKillText->SetText(FText::FromString(EnemyKillString));
	TimeTakenToClearText->SetText(FText::FromString(TimeTakenToClearString));
	TotalScoreText->SetText(FText::FromString(TotalScoreString));
}
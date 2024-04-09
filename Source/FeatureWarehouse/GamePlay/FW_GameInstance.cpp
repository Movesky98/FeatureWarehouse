// Fill out your copyright notice in the Description page of Project Settings.


#include "FW_GameInstance.h"
#include "Widgets/PlayerMenu.h"
#include "Widgets/SystemMenu.h"
#include "Widgets/GameEndMenu.h"

UFW_GameInstance::UFW_GameInstance(const FObjectInitializer& ObjectInitializer)
{

}

void UFW_GameInstance::Init()
{
	Super::Init();

	if (!PlayerClass) return;

	PlayerMenu = CreateWidget<UPlayerMenu>(this, PlayerClass);

	if (!SystemWidgetClass) return;

	SystemMenu = CreateWidget<USystemMenu>(this, SystemWidgetClass);

	if (!GameEndClass) return;

	GameEndMenu = CreateWidget<UGameEndMenu>(this, GameEndClass);
}

void UFW_GameInstance::LoadPlayerMenu()
{
	if (!PlayerMenu) return;

	PlayerMenu->SetUp();
}

void UFW_GameInstance::LoadSystemMenu()
{
	if (!SystemMenu) return;

	if (SystemMenu->IsInViewport())
	{
		SystemMenu->TearDown();
	}
	else
	{
		SystemMenu->SetUp();
	}
}

void UFW_GameInstance::LoadGameEndMenu()
{
	if (!GameEndMenu)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance || Can't GameEndMenu."));
		return;
	}

	GameEndMenu->SetUp();
}

void UFW_GameInstance::GameOver(int EnemyKills, float GameDuration, float TotalScore)
{
	if (!GameEndMenu) return;

	GameEndMenu->SetUp();
	GameEndMenu->SaveGameOverInfo(EnemyKills, GameDuration, TotalScore);
	GameEndMenu->LoadDeadPanel();
}

void UFW_GameInstance::GameClear(int EnemyKills, float GameDuration, float TotalScore)
{
	if (!GameEndMenu) return;

	GameEndMenu->SetUp();
	GameEndMenu->SetIsClearTheGame(true);
	GameEndMenu->SaveGameOverInfo(EnemyKills, GameDuration, TotalScore);
	GameEndMenu->LoadGameEndPanel();
}
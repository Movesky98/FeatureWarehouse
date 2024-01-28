// Fill out your copyright notice in the Description page of Project Settings.


#include "FW_GameInstance.h"
#include "Widgets/PlayerMenu.h"
#include "Widgets/SystemMenu.h"

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
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString("SystemMenu added to viewport."));
	}
}
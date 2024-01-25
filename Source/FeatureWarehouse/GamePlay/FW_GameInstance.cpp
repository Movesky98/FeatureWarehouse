// Fill out your copyright notice in the Description page of Project Settings.


#include "FW_GameInstance.h"
#include "Widgets/PlayerMenu.h"

UFW_GameInstance::UFW_GameInstance(const FObjectInitializer& ObjectInitializer)
{

}

void UFW_GameInstance::LoadPlayerMenu()
{
	if (!PlayerClass) return;

	PlayerMenu = CreateWidget<UPlayerMenu>(this, PlayerClass);
	if (!PlayerMenu) return;

	PlayerMenu->SetUp();
}
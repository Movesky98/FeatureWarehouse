// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMenu.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

UPlayerMenu::UPlayerMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

bool UPlayerMenu::Initialize()
{
	bool IsSuccess = Super::Initialize();

	if (!IsSuccess) return false;

	if (!HP_BarImage) return false;
	if (!Armor_BarImage) return false;
	if (!HP_ProgressBar) return false;
	if (!Armor_ProgressBar) return false;
	if (!Minimap) return false;
	if (!Crosshair) return false;
	if (!CurRoundsTextBlock) return false;
	if (!CurRoundsTextBlock) return false;

	return true;
}

void UPlayerMenu::SetUp()
{
	this->AddToViewport();

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	FInputModeGameOnly InputModeData;

	// Set input mode to GameOnly.
	PlayerController->SetInputMode(InputModeData);
	PlayerController->SetShowMouseCursor(false);
}

void UPlayerMenu::UpdatePlayerHealth(float CurHealth, float MaxHealth)
{
	if (MaxHealth == 0.0f || CurHealth > MaxHealth) return;

	float HealthPercent = CurHealth / MaxHealth;

	HP_ProgressBar->SetPercent(HealthPercent);
}
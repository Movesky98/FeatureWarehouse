// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMenu.h"

#include "Characters/PlayerCharacter.h"
#include "Components/StatComponent.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "Blueprint/WidgetLayoutLibrary.h"

UPlayerMenu::UPlayerMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

bool UPlayerMenu::Initialize()
{
	bool IsSuccess = Super::Initialize();

	if (!IsSuccess) return false;

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

	SetRPGPanel();
}

void UPlayerMenu::UpdatePlayerHealth(float CurHealth, float MaxHealth)
{
	if (MaxHealth == 0.0f || CurHealth > MaxHealth) return;

	float HealthPercent = CurHealth / MaxHealth;

	HP_ProgressBar->SetPercent(HealthPercent);
}

void UPlayerMenu::SetStatBarSize()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		UStatComponent* StatComponent = PlayerCharacter->GetStatComponent();

		if (!StatComponent) return;
		
		UCanvasPanelSlot* PanelSlot;
		// RPG HP Bar
		PanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(RPG_HP_Bar);
		PanelSlot->SetSize(FVector2D(StatComponent->GetMaxHP() * 3, PanelSlot->GetSize().Y));

		// RPG MP Bar
		PanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(RPG_MP_Bar);
		PanelSlot->SetSize(FVector2D(StatComponent->GetMaxMP() * 3, PanelSlot->GetSize().Y));

		// RPG Stamina Bar
		PanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(RPG_Stamina_Bar);
		PanelSlot->SetSize(FVector2D(StatComponent->GetMaxStamina() * 3, PanelSlot->GetSize().Y));
	}
}

void UPlayerMenu::ChangeWeaponImage(UTexture2D* Icon)
{
	switch (UIType)
	{
	case ETypeOfPlayerUI::ETP_RPGUI:
		RPG_WeaponImage->SetBrushFromTexture(Icon);
		break;
	case ETypeOfPlayerUI::ETP_ShootingUI:
		Shooting_WeaponImage->SetBrushFromTexture(Icon);
		break;
	}
}

void UPlayerMenu::SetRPGPanel()
{
	UISwitcher->SetActiveWidget(RPGGamePanel);
	UIType = ETypeOfPlayerUI::ETP_RPGUI;
}

void UPlayerMenu::SetShootingPanel()
{
	UISwitcher->SetActiveWidget(ShootingGamePanel);
	UIType = ETypeOfPlayerUI::ETP_ShootingUI;
}
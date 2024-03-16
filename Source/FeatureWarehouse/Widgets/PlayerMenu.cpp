// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMenu.h"

#include "Characters/PlayerCharacter.h"
#include "Components/StatComponent.h"

#include "Components/CanvasPanelSlot.h"
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
}

void UPlayerMenu::UpdatePlayerHealth(float CurHealth, float MaxHealth)
{
	if (MaxHealth == 0.0f || CurHealth > MaxHealth) return;

	float HealthPercent = CurHealth / MaxHealth;

	HP_ProgressBar->SetPercent(HealthPercent);
}

void UPlayerMenu::SetStatBarSize()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("PlayerMenu :: Set stat bar size."));
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		UStatComponent* StatComponent = PlayerCharacter->GetStatComponent();

		if (!StatComponent)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("PlayerMenu :: Can't set Bar size. Because the stat component is not valid."));
			return;
		}
		
		UCanvasPanelSlot* PanelSlot;
		// RPG HP Bar
		PanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(RPG_HP_Bar);
		PanelSlot->SetSize(FVector2D(StatComponent->GetMaxHP() * 3, PanelSlot->GetSize().Y));
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("HP Bar X Size : ") + FString::SanitizeFloat(StatComponent->GetMaxHP() * 3));

		// RPG MP Bar
		PanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(RPG_MP_Bar);
		PanelSlot->SetSize(FVector2D(StatComponent->GetMaxMP() * 3, PanelSlot->GetSize().Y));
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("MP Bar X Size : ") + FString::SanitizeFloat(StatComponent->GetMaxMP() * 3));

		// RPG Stamina Bar
		PanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(RPG_Stamina_Bar);
		PanelSlot->SetSize(FVector2D(StatComponent->GetMaxStamina() * 3, PanelSlot->GetSize().Y));
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("Stamina Bar X Size : ") + FString::SanitizeFloat(StatComponent->GetMaxStamina() * 3));
	}
}

void UPlayerMenu::SetRPGPanel()
{
	UIType = ETypeOfPlayerUI::ETP_RPGGame;
}

void UPlayerMenu::SetShootingPanel()
{
	UIType = ETypeOfPlayerUI::ETP_ShootingGame;
}
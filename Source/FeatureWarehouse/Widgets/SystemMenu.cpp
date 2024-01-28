// Fill out your copyright notice in the Description page of Project Settings.


#include "SystemMenu.h"

#include "Characters/PlayerCharacter.h"

#include "Components/CanvasPanel.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"

#include "Kismet/GamePlayStatics.h"

USystemMenu::USystemMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

bool USystemMenu::Initialize()
{
	Super::Initialize();

	if (!ChangePerspectiveButton) return false;
	if (!GameExitButton) return false;
	if (!FPPButton) return false;
	if (!TPPButton) return false;
	if (!TDPButton) return false;
	if (!CP_BackButton) return false;

	ChangePerspectiveButton->OnClicked.AddDynamic(this, &USystemMenu::ViewChangePerspectivePanel);
	CP_BackButton->OnClicked.AddDynamic(this, &USystemMenu::BackToDefaultPanel);
	GameExitButton->OnClicked.AddDynamic(this, &USystemMenu::GameExit);
	FPPButton->OnClicked.AddDynamic(this, &USystemMenu::SetFPP);
	TPPButton->OnClicked.AddDynamic(this, &USystemMenu::SetTPP);
	TDPButton->OnClicked.AddDynamic(this, &USystemMenu::SetTDP);

	return true;
}

void USystemMenu::BackToDefaultPanel()
{
	if (UISwitcher->GetActiveWidget() != DefaultPanel)
	{
		UISwitcher->SetActiveWidget(DefaultPanel);
	}
}

void USystemMenu::ViewChangePerspectivePanel()
{
	if (UISwitcher->GetActiveWidget() != ChangePerspectivePanel)
	{
		UISwitcher->SetActiveWidget(ChangePerspectivePanel);
	}
}

void USystemMenu::GameExit()
{
	UWorld* World = GetWorld();

	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		PlayerController->ConsoleCommand("quit");
	}
}

void USystemMenu::SetFPP()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (PlayerCharacter)
	{
		PlayerCharacter->SetFPP();
	}
}

void USystemMenu::SetTPP()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (PlayerCharacter)
	{
		PlayerCharacter->SetTPP();
	}
}

void USystemMenu::SetTDP()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (PlayerCharacter)
	{
		PlayerCharacter->SetTDP();
	}
}

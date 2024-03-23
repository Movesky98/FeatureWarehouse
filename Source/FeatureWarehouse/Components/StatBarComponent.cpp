// Fill out your copyright notice in the Description page of Project Settings.


#include "StatBarComponent.h"

#include "Characters/Wielder.h"
#include "Widgets/StatBar.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

UStatBarComponent::UStatBarComponent()
{
	static ConstructorHelpers::FClassFinder<UStatBar> WBP_StatBar(TEXT("/Game/Project/Blueprints/Widget/WBP_StatBar"));
	if (WBP_StatBar.Succeeded())
	{
		SetWidgetClass(WBP_StatBar.Class);
	}
}

void UStatBarComponent::Init()
{
	InitWidget();
	StatBarWidget = Cast<UStatBar>(GetUserWidgetObject());

	if (!ensure(StatBarWidget != nullptr)) return;

	AWielder* Wielder = Cast<AWielder>(GetOwner());

	if(IsValid(Wielder)) StatBarWidget->Init(Wielder);

	ShowUI();
}

void UStatBarComponent::LookAtPlayer()
{
	FVector Start = GetOwner()->GetActorLocation();
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	
	ensureMsgf(PlayerCharacter != nullptr, TEXT("PlayerCharacter is invalid"));

	FVector Target = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();

	FRotator GoalRotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);

	SetWorldRotation(GoalRotation);
}

void UStatBarComponent::ShowUI()
{
	StatBarWidget->SetVisibility(ESlateVisibility::Visible);
	bIsShowing = true;
	GetWorld()->GetTimerManager().SetTimer(ShowingTimer, this, &UStatBarComponent::LookAtPlayer, 0.01f, true);
}

void UStatBarComponent::HideUI()
{
	StatBarWidget->SetVisibility(ESlateVisibility::Hidden);
	bIsShowing = false;
	GetWorld()->GetTimerManager().ClearTimer(ShowingTimer);
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "LockOnComponent.h"


ULockOnComponent::ULockOnComponent()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> BP_LockOn(TEXT("/Game/Project/Blueprints/Widget/WBP_LockOn"));
	if (BP_LockOn.Succeeded())
	{
		SetWidgetClass(BP_LockOn.Class);
	}

	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawSize(FVector2D(30.0f, 30.0f));
}

void ULockOnComponent::Init()
{
	InitWidget();
	LockOnWidget = GetUserWidgetObject();

	HideUI();
}

void ULockOnComponent::ShowUI()
{
	LockOnWidget->SetVisibility(ESlateVisibility::Visible);
}

void ULockOnComponent::HideUI()
{
	LockOnWidget->SetVisibility(ESlateVisibility::Hidden);
}
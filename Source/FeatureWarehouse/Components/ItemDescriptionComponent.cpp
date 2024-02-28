// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDescriptionComponent.h"
#include "Widgets/ItemDescriptionWidget.h"

UItemDescriptionComponent::UItemDescriptionComponent()
{
	static ConstructorHelpers::FClassFinder<UItemDescriptionWidget> BP_ItemDescriptionWidget(TEXT("/Game/Project/Blueprints/Widget/WBP_ItemDescriptionWidget"));
	if (BP_ItemDescriptionWidget.Succeeded())
	{
		SetWidgetClass(BP_ItemDescriptionWidget.Class);
	}

	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawSize(FVector2D(500.0f, 300.0f));
}

void UItemDescriptionComponent::Init()
{
	InitWidget();
	ItemDescriptionWidget = Cast<UItemDescriptionWidget>(GetUserWidgetObject());

	if (!ensure(ItemDescriptionWidget != nullptr)) return;

	ItemDescriptionWidget->InitUI(Name, WeaponType, Icon);
	HideUI();
}

void UItemDescriptionComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UItemDescriptionComponent::ShowUI()
{
	ItemDescriptionWidget->SetVisibility(ESlateVisibility::Visible);
}

void UItemDescriptionComponent::HideUI()
{
	ItemDescriptionWidget->SetVisibility(ESlateVisibility::Hidden);
}
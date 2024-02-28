// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDescriptionWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

UItemDescriptionWidget::UItemDescriptionWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

bool UItemDescriptionWidget::Initialize()
{
	bool IsSuccess = Super::Initialize();

	if (!ensure(IconImage != nullptr)) return false;
	if (!ensure(NameText != nullptr)) return false;
	if (!ensure(WeaponTypeText != nullptr)) return false;

	return true;
}

void UItemDescriptionWidget::InitUI(FString Name, FString WeaponType, UTexture2D* Image)
{
	if (!IsValid(Image)) return;

	FString NameString = FString("Name : ") + Name;
	FString WeaponTypeString = FString("Weapon Type : ") + WeaponType;

	IconImage->SetBrushFromTexture(Image);
	NameText->SetText(FText::FromString(NameString));
	WeaponTypeText->SetText(FText::FromString(WeaponTypeString));
}
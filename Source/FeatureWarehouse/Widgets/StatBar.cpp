// Fill out your copyright notice in the Description page of Project Settings.


#include "StatBar.h"

#include "Characters/Wielder.h"
#include "Components/StatComponent.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UStatBar::Init(class AWielder* NewOwner)
{
	OwningWielder = NewOwner;

	FString Name;

	switch (OwningWielder->GetWielderType())
	{
	case ETypeOfWielder::ETW_Samurai:
		Name = FString("Samurai");
		break;
	case ETypeOfWielder::ETW_Halberdier:
		Name = FString("Halberdier");
		break;
	case ETypeOfWielder::ETW_Warrior:
		Name = FString("Warrior");
		break;
	default:
		Name = FString("NoName");
		break;
	}

	HP_Bar->SetPercent(1.0f);
	DamageAccumulation = 0.0f;
	CharacterNameText->SetText(FText::FromString(Name));
}

void UStatBar::UpdateHPBar(float BarPercent, float Damage)
{
	DamageAccumulation += Damage;

	// Update UI.
	HP_Bar->SetPercent(BarPercent);
	DamageAmountText->SetText(FText::FromString(FString::FromInt(DamageAccumulation)));
}

void UStatBar::CleanDamageAccumulation()
{
	DamageAccumulation = 0.0f;
	DamageAmountText->SetText(FText::GetEmpty());
}
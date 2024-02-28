// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "ItemDescriptionComponent.generated.h"

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API UItemDescriptionComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	UItemDescriptionComponent();

	void Init();

	void ShowUI();

	void HideUI();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Description", meta = (AllowPrivateAccess = "true"))
	FString Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Description", meta = (AllowPrivateAccess = "true"))
	FString WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Description", meta = (AllowPrivateAccess = "true"))
	UTexture2D* Icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = "true"))
	class UItemDescriptionWidget* ItemDescriptionWidget;
};

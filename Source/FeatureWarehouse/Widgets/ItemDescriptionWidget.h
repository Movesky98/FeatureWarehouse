// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemDescriptionWidget.generated.h"

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API UItemDescriptionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UItemDescriptionWidget(const FObjectInitializer& ObjectInitializer);

	void InitUI(FString Name, FString WeaponType, UTexture2D* Image);

protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WeaponTypeText;
};

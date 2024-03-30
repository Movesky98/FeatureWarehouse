// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "LockOnComponent.generated.h"

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API ULockOnComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	ULockOnComponent();

	void Init();

	void ShowUI();

	void HideUI();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
	UUserWidget* LockOnWidget;
};

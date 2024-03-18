// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "StatBarComponent.generated.h"

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API UStatBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	UStatBarComponent();

	void Init();

	void ShowUI();

	void HideUI();

	bool IsShowing()
	{
		return bIsShowing;
	}

protected:
	UFUNCTION()
	void LookAtPlayer();

	FTimerHandle ShowingTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsShowing;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = "true"))
	class UStatBar* StatBarWidget;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FW_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API UFW_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFW_GameInstance(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void LoadPlayerMenu();

	UFUNCTION(BlueprintCallable)
	void LoadSystemMenu();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget)
	class UPlayerMenu* PlayerMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget)
	class USystemMenu* SystemMenu;

protected:
	void Init() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> PlayerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> SystemWidgetClass;
};

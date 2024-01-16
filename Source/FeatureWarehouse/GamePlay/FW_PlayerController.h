// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FW_PlayerController.generated.h"

enum class EStateOfViews :uint8;
/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API AFW_PlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AFW_PlayerController();

	void ViewClickLocation();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	EStateOfViews ViewState;

#pragma region GetterSetter
public:
	FORCEINLINE void SetView(EStateOfViews NewState) { ViewState = NewState; }
	FORCEINLINE EStateOfViews GetView() { return ViewState; }

#pragma endregion
};

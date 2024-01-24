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

	FVector ViewClickLocation();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	EStateOfViews ViewState;

#pragma region GetterSetter
public:
	FORCEINLINE void SetPerspective(EStateOfViews NewState) { ViewState = NewState; }
	FORCEINLINE EStateOfViews GetPerspective() { return ViewState; }

#pragma endregion
};

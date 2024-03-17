// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
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
	
	void SwitchPlayerMenu();

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	void ShowSystemMenu(const FInputActionValue& Value);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	EStateOfViews ViewState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = InputAction, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SystemMenuAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InputAction, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu", meta = (AllowPrivateAccess = "true"))
	class UPlayerMenu* PlayerMenu;

#pragma region GetterSetter
public:
	FORCEINLINE void SetPerspective(EStateOfViews NewState) { ViewState = NewState; }
	FORCEINLINE EStateOfViews GetPerspective() { return ViewState; }

#pragma endregion
};

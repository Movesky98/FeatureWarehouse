// Fill out your copyright notice in the Description page of Project Settings.


#include "FW_PlayerController.h"
#include "Enums/StateOfViews.h"
#include "GamePlay/FW_GameInstance.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathlibrary.h"
#include "DrawDebugHelpers.h"

AFW_PlayerController::AFW_PlayerController()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AFW_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	UFW_GameInstance* GameInstance = Cast<UFW_GameInstance>(GetGameInstance());

	if (GameInstance)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("Load player menu."));
		GameInstance->LoadPlayerMenu();
	}
}

void AFW_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(SystemMenuAction, ETriggerEvent::Triggered, this, &AFW_PlayerController::ShowSystemMenu);
	}
}

void AFW_PlayerController::ShowSystemMenu(const FInputActionValue& Value)
{
	bool IsPressed = Value.Get<bool>();

	if (IsPressed)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString("Show System Menu."));
		UFW_GameInstance* GameInstance = Cast<UFW_GameInstance>(GetGameInstance());

		if (GameInstance)
		{
			GameInstance->LoadSystemMenu();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString("Can't cast to UFW Gameinstance."));
		}
	}
}

FVector AFW_PlayerController::ViewClickLocation()
{
	FHitResult Hit;
	bool IsHit = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

	if (IsHit)
	{
		FVector Target = Hit.ImpactPoint - GetPawn()->GetActorLocation();

		FRotator TargetRotation = GetPawn()->GetActorRotation();
		TargetRotation.Yaw = UKismetMathLibrary::MakeRotFromX(Target).Yaw;

		GetPawn()->SetActorRotation(TargetRotation);

		DrawDebugSolidBox(GetWorld(), Hit.Location, FVector(3.0f, 3.0f, 3.0f), FColor::Red, false, 3.0f);

		return Hit.Location;
	}

	return FVector(0.0f);
}
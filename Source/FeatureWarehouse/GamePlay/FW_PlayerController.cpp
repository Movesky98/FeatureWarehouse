// Fill out your copyright notice in the Description page of Project Settings.


#include "FW_PlayerController.h"
#include "Enums/StateOfViews.h"

#include "Kismet/KismetMathlibrary.h"
#include "DrawDebugHelpers.h"

AFW_PlayerController::AFW_PlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AFW_PlayerController::Tick(float DeltaSeconds)
{
	switch (ViewState)
	{
	case EStateOfViews::TPV:
		break;
	case EStateOfViews::FPV:
		break;
	case EStateOfViews::TopView:
	{
		FHitResult Hit;
		bool IsHit = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

		if (IsHit)
		{
			FVector Target = Hit.ImpactPoint - GetPawn()->GetActorLocation();

			FRotator TargetRotation = GetPawn()->GetActorRotation();
			TargetRotation.Yaw = UKismetMathLibrary::MakeRotFromX(Target).Yaw;

			SetControlRotation(TargetRotation);

			DrawDebugSolidBox(GetWorld(), Hit.Location, FVector(3.0f, 3.0f, 3.0f), FColor::Green, false, 3.0f);
		}
	
	}
		break;
	default:
		break;
	}
}

void AFW_PlayerController::ViewClickLocation()
{
	/*FHitResult Hit;
	bool IsHit = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

	if (IsHit)
	{
		FVector Start = GetPawn()->GetActorLocation();
		FVector Target = Hit.Location;
		FRotator TargetRotation = GetPawn()->GetActorRotation();

		TargetRotation.Yaw = UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, Target).Yaw;

		SetControlRotation(TargetRotation);

		DrawDebugSolidBox(GetWorld(), Hit.Location, FVector(3.0f, 3.0f, 3.0f), FColor::Green, false, 3.0f);

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TargetRotation.ToString());
	}*/
}
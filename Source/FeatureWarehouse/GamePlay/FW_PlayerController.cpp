// Fill out your copyright notice in the Description page of Project Settings.


#include "FW_PlayerController.h"
#include "Enums/StateOfViews.h"

#include "Kismet/KismetMathlibrary.h"
#include "DrawDebugHelpers.h"

AFW_PlayerController::AFW_PlayerController()
{
	PrimaryActorTick.bCanEverTick = false;

}


void AFW_PlayerController::ViewClickLocation()
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
	}
}
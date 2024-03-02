#pragma once

#include "CoreMinimal.h"
#include "MovementState.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	EMS_Idle UMETA(DisplayName = "Idle"),
	EMS_Walking UMETA(DisplayName = "Walking"),
	EMS_Running UMETA(DisplayName = "Running"),
	EMS_Jumping UMETA(DisplayName = "Jumping"),
	EMS_Crouching UMETA(DisplayName = "Crouching")
};
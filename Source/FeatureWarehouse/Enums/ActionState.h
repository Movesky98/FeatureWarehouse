#pragma once

#include "CoreMinimal.h"
#include "ActionState.generated.h"

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Idle UMETA(DisplayName = "Idle"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_HeavyAttacking UMETA(DisplayName = "Heavy Attacking"),
	EAS_Swapping UMETA(DisplayName = "Swapping")
};
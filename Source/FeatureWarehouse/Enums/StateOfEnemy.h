#pragma once

#include "CoreMinimal.h"
#include "StateOfEnemy.generated.h"

UENUM(BlueprintType)
enum class EStateOfEnemy : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Patrol UMETA(DisplayName = "Patrol"),
	Check UMETA(DisplayName = "Check"),
	In_Battle UMETA(DisplayName = "In_Battle"),
	Chase UMETA(DisplayName = "Chase"),
	Flee UMETA(DisplayName = "Flee")
};
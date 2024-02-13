#pragma once

#include "CoreMinimal.h"
#include "StateOfEnemy.generated.h"

UENUM(BlueprintType)
enum class EStateOfEnemy : uint8
{
	Patrol UMETA(DisplayName = "Patrol"),
	In_Battle UMETA(DisplayName = "In_Battle"),
	Chase UMETA(DisplayName = "Chase"),
	Flee UMETA(DisplayName = "Flee")
};
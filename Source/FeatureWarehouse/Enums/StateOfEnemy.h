#pragma once

#include "CoreMinimal.h"
#include "StateOfEnemy.generated.h"

UENUM(BlueprintType)
enum class EStateOfEnemy : uint8
{
	Patrol UMETA(DisplayName = "Patrol"),
	Chase UMETA(DisplayName = "Chase"),
	Avoid UMETA(DisplayName = "Avoid")
};
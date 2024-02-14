#pragma once

#include "CoreMinimal.h"
#include "BattleState.generated.h"

UENUM(BlueprintType)
enum class EBattleState : uint8
{
	None UMETA(DisplayName = "None"),
	Monitoring UMETA(DisplayName = "Monitoring"),	// 경게하다.
	Approaching UMETA(DisplayName = "Approaching"),	// 접근하다.
	Attacking UMETA(DisplayName = "Attacking"),
	Retreating UMETA(DisplayName = "Retreating")	// 뒤로 물리다.
};
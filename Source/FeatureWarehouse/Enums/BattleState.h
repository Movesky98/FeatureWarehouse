#pragma once

#include "CoreMinimal.h"
#include "BattleState.generated.h"

UENUM(BlueprintType)
enum class EBattleState : uint8
{
	None UMETA(DisplayName = "None"),
	Monitoring UMETA(DisplayName = "Monitoring"),	// 경게
	Approaching UMETA(DisplayName = "Approaching"),	// 접근
	Attacking UMETA(DisplayName = "Attacking"),		// 공격
	Retreating UMETA(DisplayName = "Retreating")	// 뒤로 물림
};
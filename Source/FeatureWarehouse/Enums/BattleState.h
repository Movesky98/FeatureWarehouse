#pragma once

#include "CoreMinimal.h"
#include "BattleState.generated.h"

UENUM(BlueprintType)
enum class EBattleState : uint8
{
	None UMETA(DisplayName = "None"),				// 전투 상태가 아닌 상태
	Monitoring UMETA(DisplayName = "Monitoring"),	// 경계 상태
	Approaching UMETA(DisplayName = "Approaching"),	// 접근 상태
	Attacking UMETA(DisplayName = "Attacking"),		// 공격 상태
	Retreating UMETA(DisplayName = "Retreating")	// 뒤로 물리는 상태 (회피)
};
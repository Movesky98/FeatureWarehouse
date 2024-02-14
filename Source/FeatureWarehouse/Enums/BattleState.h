#pragma once

#include "CoreMinimal.h"
#include "BattleState.generated.h"

UENUM(BlueprintType)
enum class EBattleState : uint8
{
	None UMETA(DisplayName = "None"),
	Monitoring UMETA(DisplayName = "Monitoring"),	// ����ϴ�.
	Approaching UMETA(DisplayName = "Approaching"),	// �����ϴ�.
	Attacking UMETA(DisplayName = "Attacking"),
	Retreating UMETA(DisplayName = "Retreating")	// �ڷ� ������.
};
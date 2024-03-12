#pragma once

#include "CoreMinimal.h"
#include "BattleState.generated.h"

UENUM(BlueprintType)
enum class EBattleState : uint8
{
	None UMETA(DisplayName = "None"),				// ���� ���°� �ƴ�
	Monitoring UMETA(DisplayName = "Monitoring"),	// ���
	Approaching UMETA(DisplayName = "Approaching"),	// ����
	Attacking UMETA(DisplayName = "Attacking"),		// ����
	Retreating UMETA(DisplayName = "Retreating")	// �ڷ� ����
};
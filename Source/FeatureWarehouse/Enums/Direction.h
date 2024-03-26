#pragma once

#include "CoreMinimal.h"
#include "Direction.generated.h"

UENUM(BlueprintType)
enum class EDirection : uint8
{
	ED_Forward UMETA(DisplayName = "Forward"),
	ED_LeftForward UMETA(DisplayName = "Left Forward"),
	ED_RightForward UMETA(DisplayName = "Right Forward"),
	ED_Right UMETA(DisplayName = "Right"),
	ED_RightBackward UMETA(DisplayName = "Right Backward"),
	ED_Backward UMETA(DisplayName = "Backward"),
	ED_LeftBackward UMETA(DisplayName = "Left Backward"),
	ED_Left UMETA(DisplayName = "Left")
};
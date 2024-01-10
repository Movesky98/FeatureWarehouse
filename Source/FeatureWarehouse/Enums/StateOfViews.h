#pragma once

#include "UObject/ObjectMacros.h"
#include "StateOfViews.generated.h"

UENUM(BlueprintType)
enum class EStateOfViews : uint8
{
	TPV UMETA(DisplayName = "TPV"),
	FPV UMETA(DisplayName = "FPV"),
	TopView UMETA(DisplayName = "TopView")
};
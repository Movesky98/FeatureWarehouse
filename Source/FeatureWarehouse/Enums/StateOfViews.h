#pragma once

#include "CoreMinimal.h"
#include "StateOfViews.generated.h"

UENUM(BlueprintType)
enum class EStateOfViews : uint8
{
	TPP UMETA(DisplayName = "TPP"),
	FPP UMETA(DisplayName = "FPP"),
	TDP UMETA(DisplayName = "TDP")
};
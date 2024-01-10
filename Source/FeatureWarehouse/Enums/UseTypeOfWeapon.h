#pragma once

#include "UObject/ObjectMacros.h"
#include "UseTypeOfWeapon.generated.h"

UENUM(BlueprintType)
enum class EUseTypeOfWeapon : uint8
{
	Main UMETA(DisplayName = "Main"),
	Sub UMETA(DisplayName = "Sub"),
	None UMETA(DisplayName = "None")
};
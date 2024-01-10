#pragma once

#include "UObject/ObjectMacros.h"
#include "TypeOfItem.generated.h"

UENUM(BlueprintType)
enum class ETypeOfItem : uint8
{
	Weapon UMETA(DisplayName = "Weapon")
};
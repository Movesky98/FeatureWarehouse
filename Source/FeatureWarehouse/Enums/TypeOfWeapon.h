#pragma once

#include "UObject/ObjectMacros.h"
#include "TypeOfWeapon.generated.h"

UENUM(BlueprintType)
enum class ETypeOfWeapon : uint8
{
	Gun UMETA(DisplayName = "Gun"),
	Melee UMETA(DisplayName = "Melee"),
	Wand UMETA(DisplayName = "Wand"),
	Halberd UMETA(DisplayName = "Halberd")
};
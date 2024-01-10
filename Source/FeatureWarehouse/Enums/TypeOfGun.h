#pragma once

#include "CoreMinimal.h"
#include "TypeOfGun.generated.h"

UENUM(BlueprintType)
enum class ETypeOfGun : uint8
{
	Rifle UMETA(DisplayName = "Rifle"),
	Pistol UMETA(DisplayName = "Pistol"),
	Shotgun UMETA(DisplayName = "Shotgun")
};
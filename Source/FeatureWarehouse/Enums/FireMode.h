#pragma once

#include "CoreMinimal.h"
#include "FireMode.generated.h"

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	SemiAuto UMETA(DisplayName = "SemiAuto"),
	FullAuto UMETA(DisplayName = "FullAuto"),
	Burst UMETA(DisplayName = "Burst")
};
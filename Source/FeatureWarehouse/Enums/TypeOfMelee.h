#pragma once

#include "CoreMinimal.h"
#include "TypeOfMelee.generated.h"

UENUM(BlueprintType)
enum class ETypeOfMelee : uint8
{
	None UMETA(DisplayName = "None"),
	Katana UMETA(DisplayName = "Katana"),
	SwordAndBoard UMETA(DisplayName = "SwordAndBoard")
};
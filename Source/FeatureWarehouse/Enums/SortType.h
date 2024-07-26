#pragma once

#include "CoreMinimal.h"
#include "SortType.generated.h"

UENUM(BlueprintType)
enum class ESortType : uint8
{
	ST_Selection = 0 UMETA(DisplayName = "Selection"),
	ST_Insertion UMETA(DisplayName = "Insertion"),
	ST_Bubble UMETA(DisplayName = "Bubble"),
	ST_Quick UMETA(DisplayName = "Quick"),
	ST_Heap UMETA(DisplayName = "Heap"),
	ST_Shell UMETA(DisplayName = "Shell"),
	ST_Radix UMETA(DisplayName = "Radix"),
	ST_Counting UMETA(DisplayName = "Counting")
};
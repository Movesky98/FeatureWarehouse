#pragma once

#include "CoreMinimal.h"
#include "PoseType.generated.h"

UENUM(BlueprintType)
enum class EPoseType : uint8
{
	EPT_Stand UMETA(DisplayName = "Stand"),
	EPT_Crouch UMETA(DisplayName = "Crouch"),
	EPT_Crawl UMETA(DisplayName = "Crawl"),
	EPT_KnockDown UMETA(DisplayName = "KnockDown")
};
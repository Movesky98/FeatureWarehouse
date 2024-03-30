#pragma once

#include "CoreMinimal.h"
#include "FactionType.generated.h"

UENUM(BlueprintType)
enum class EFactionType : uint8
{
	EFT_None = 0 UMETA(DisplayName = "None"),
	EFT_PlayerFriendly = 1 UMETA(DisplayName = "PlayerFriendly"),
	EFT_Samurai = 2 UMETA(DisplayName = "Samurai"),
	EFT_Halberdier = 3 UMETA(DisplayName = "Halberdier"),
	EFT_Warrior = 4 UMETA(DisplayName = "Warrior"),
	EFT_Neutral = 255 UMETA(DisplayName = "Neutral")
};
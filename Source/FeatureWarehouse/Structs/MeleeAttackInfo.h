#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Sound/SoundCue.h"
#include "MeleeAttackInfo.generated.h"

USTRUCT(BlueprintType)
struct FMeleeAttackInfo : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Damage", MakeStructureDefaultValue = "0.000000"))
	float m_Damage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "StaminaCost", MakeStructureDefaultValue = "0.000000"))
	float m_StaminaCost;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Sound"))
	USoundCue* m_Sound;
};

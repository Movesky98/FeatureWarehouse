#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Enums/TypeOfMelee.h"
#include "Structs/MeleeAttackInfo.h"
#include "Structs/MeleeMontageInfo.h"
#include "MeleeInfo.generated.h"

USTRUCT(BlueprintType)
struct FMeleeInfo : public FTableRowBase
{
	GENERATED_BODY()
public:
	/** 근접 무기 타입 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "MeleeType", MakeStructureDefaultValue = "None"))
	ETypeOfMelee D_MeleeType;

	/** Melee's LightAttackInfo */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "LightAttackInfo"))
	TArray<FMeleeAttackInfo> m_LightAttackInfo;

	/** Melee's HeavyAttackInfo */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "HeavyAttackInfo"))
	TArray<FMeleeAttackInfo> m_HeavyAttackInfo;

	/** Melee's SprintAttackInfo */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "SprintAttackInfo"))
	FMeleeAttackInfo m_SprintAttackInfo;

	/** Melee's JumpAttackInfo */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "JumpAttackInfo"))
	FMeleeAttackInfo m_JumpAttackInfo;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "GroggyAttackInfo"))
	FMeleeAttackInfo m_GroggyAttackInfo;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "MontageInfo"))
	FMeleeMontageInfo m_MontageInfo;
};
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Enums/TypeOfMelee.h"

#include "MeleeDamage.generated.h"

/** 근접 무기의 데미지를 저장하기 위한 구조체 */
USTRUCT(BlueprintType)
struct FMeleeDamage : public FTableRowBase
{
	GENERATED_BODY()
public:
	/** 근접 무기 이름 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "MeleeType", MakeStructureDefaultValue = "None"))
	ETypeOfMelee D_MeleeType;

	/** 약공격 데미지 TArray */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "LightAttackDamage"))
	TArray<float> D_LightAttackDamage;

	/** 강공격 데미지 TArray */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "HeavyAttackDamage"))
	TArray<float> D_HeavyAttackDamage;

	/** 스프린트 공격 데미지 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "SprintAttackDamage", MakeStructureDefaultValue = "0.000000"))
	float D_SprintAttack;

	/** 점프 공격 데미지 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "JumpAttackDamage", MakeStructureDefaultValue = "0.000000"))
	float D_JumpAttack;
};

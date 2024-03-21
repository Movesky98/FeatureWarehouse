#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Enums/TypeOfMelee.h"

#include "MeleeDamage.generated.h"

/** ���� ������ �������� �����ϱ� ���� ����ü */
USTRUCT(BlueprintType)
struct FMeleeDamage : public FTableRowBase
{
	GENERATED_BODY()
public:
	/** ���� ���� �̸� */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "MeleeType", MakeStructureDefaultValue = "None"))
	ETypeOfMelee D_MeleeType;

	/** ����� ������ TArray */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "LightAttackDamage"))
	TArray<float> D_LightAttackDamage;

	/** ������ ������ TArray */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "HeavyAttackDamage"))
	TArray<float> D_HeavyAttackDamage;

	/** ������Ʈ ���� ������ */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "SprintAttackDamage", MakeStructureDefaultValue = "0.000000"))
	float D_SprintAttack;

	/** ���� ���� ������ */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "JumpAttackDamage", MakeStructureDefaultValue = "0.000000"))
	float D_JumpAttack;
};

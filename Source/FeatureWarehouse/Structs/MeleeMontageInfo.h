#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Enums/Direction.h"
#include "MeleeMontageInfo.generated.h"

USTRUCT(BlueprintType)
struct FMeleeMontageInfo : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "LightAttack"))
	UAnimMontage* m_LightAttackMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "HeavyAttack"))
	UAnimMontage* m_HeavyAttackMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "JumpAttack"))
	UAnimMontage* m_JumpAttackMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "JumpAttackLand"))
	UAnimMontage* m_JumpAttackLand;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "SprintAttack"))
	UAnimMontage* m_SprintAttackMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Groggy Attack Hit"))
	UAnimMontage* m_Groggy_Attack_Hit;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Groggy Hit Reaction"))
	UAnimMontage* m_Groggy_Hit_Reaction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Groggy Hit Death"))
	UAnimMontage* m_Groggy_Hit_Death;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Equip"))
	UAnimMontage* m_EquipMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Unequip"))
	UAnimMontage* m_UnequipMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Hit"))
	UAnimMontage* m_HitMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Death"))
	UAnimMontage* m_DeathMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Retreat"))
	UAnimMontage * m_RetreatMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Dodge"))
	TMap<EDirection, UAnimMontage*> m_DodgeMontages;
};

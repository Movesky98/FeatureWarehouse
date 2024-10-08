// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomDamageType.h"
#include "CriticalDamageType.generated.h"

class UAnimMontage;

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API UCriticalDamageType : public UCustomDamageType
{
	GENERATED_BODY()
	
public:
	void SetReactionMontage(UAnimMontage* Montage);

	UAnimMontage* GetReactionMontage() const;

	void SetDeathMontage(UAnimMontage* Montage);

	UAnimMontage* GetDeathMontage() const;

	void SetPlayLocation(FVector Location);

	FVector GetPlayLocation() const;


private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReactionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info", meta = (AllowPrivateAccess = "true"))
	FVector PlayLocation;
};

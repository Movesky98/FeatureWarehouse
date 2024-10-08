// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "CustomDamageType.generated.h"

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API UCustomDamageType : public UDamageType
{
	GENERATED_BODY()
	
public:
	void SetIsDead(uint8 IsDead);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	uint8 bIsDead:1;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICharacter.h"
#include "GoldSkull.generated.h"

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API AGoldSkull : public AAICharacter
{
	GENERATED_BODY()
public:
	AGoldSkull();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnTriggerBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};

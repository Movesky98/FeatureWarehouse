// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "GoldenSkull.generated.h"

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API AGoldenSkull : public AEnemy
{
	GENERATED_BODY()
public:
	AGoldenSkull();

	FVector FindLocationToRunAwayFromPlayer();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnTriggerBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION(BlueprintCallable)
	void CalculateFleeLocation();

	UFUNCTION(BlueprintCallable)
	FRotator CalculateTraceRotation(const FRotator& FirstRotation, const float& DeltaYaw, int Index);

	FVector GetFloorLocationOnHit(const FVector& Location);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Avoid, meta = (AllowPrivateAccess = "true"))
	int FleeTraceNum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Avoid, meta = (AllowPrivateAccess = "true"))
	float MaxAvoidanceRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Avoid, meta = (AllowPrivateAccess = "true"))
	float MinAvoidanceRange;
};

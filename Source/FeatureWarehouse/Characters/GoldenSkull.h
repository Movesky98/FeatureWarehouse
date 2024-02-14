// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "GoldenSkull.generated.h"

enum class EStateOfEnemy :uint8;

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API AGoldenSkull : public AEnemy
{
	GENERATED_BODY()
public:
	AGoldenSkull();

protected:
	virtual void BeginPlay() override;

	EStateOfEnemy IdentifiedPlayer() override;
	
	UFUNCTION()
	void OnTriggerBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	FVector CalculateFleeLocation();

	UFUNCTION(BlueprintCallable)
	FRotator CalculateTraceRotation(const FRotator& FirstRotation, const float& DeltaYaw, int Index);

	FVector GetFloorLocationOnHit(const FVector& Location);

	UFUNCTION(BlueprintCallable)
	void ReturnStateToPatrol();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Avoid, meta = (AllowPrivateAccess = "true"))
	int FleeTraceNum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Avoid, meta = (AllowPrivateAccess = "true"))
	float MaxAvoidanceRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Avoid, meta = (AllowPrivateAccess = "true"))
	float MinAvoidanceRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Avoid, meta = (AllowPrivateAccess = "true"))
	float FleeSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Avoid, meta = (AllowPrivateAccess = "true"))
	float NormalSpeed;;
};

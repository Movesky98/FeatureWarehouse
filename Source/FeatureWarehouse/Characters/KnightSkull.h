// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Enemy.h"
#include "KnightSkull.generated.h"

enum class EBattleState :uint8;

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API AKnightSkull : public AEnemy
{
	GENERATED_BODY()
public:
	AKnightSkull();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTriggerBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnAttackRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAttackRangeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnAttackEnded(class UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION(BlueprintCallable)
	void EngagingInCombat(AActor* AdversaryActor) override;

	UFUNCTION(BlueprintCallable)
	void MonitoringPlayer();

	UFUNCTION()
	void CalculateMonitoringTime();
	
	UFUNCTION(BlueprintCallable)
	FVector MaintainingDistance();

	UFUNCTION(BlueprintCallable)
	FVector CirclingAroundThePlayer();

	void Attack() override;

	UFUNCTION()
	void OnNextAttackChecked();

	UFUNCTION()
	void ReturnToPatrol();

	void ResetAttackState();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Sword;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|State", meta = (AllowPrivateAccess = "true"))
	EBattleState BattleState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|State", meta = (AllowPrivateAccess = "true"))
	float MonitoringDistance;

	FTimerHandle MonitoringTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|State", meta = (AllowPrivateAccess = "true"))
	float MonitoringTimerDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|State", meta = (AllowPrivateAccess = "true"))
	float ElapsedTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Monitoring", meta = (AllowPrivateAccess = "true"))
	bool bIsCirclingAroundThePlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Monitoring", meta = (AllowPrivateAccess = "true"))
	bool bIsStartMonitoring;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Montage", meta = (AllowPrivateAccess = "true"))
	int MontageIndex;
};

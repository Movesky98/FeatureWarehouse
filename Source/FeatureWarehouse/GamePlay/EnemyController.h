// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyController.generated.h"

enum class EStateOfEnemy :uint8;
enum class EBattleState :uint8;

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API AEnemyController : public AAIController
{
	GENERATED_BODY()
public:
	AEnemyController();

	UFUNCTION(BlueprintCallable)
	bool IsIdentifiedPlayer();

	void NotifyEnemyActor(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void NotifyEnemyState(EStateOfEnemy State);

	UFUNCTION(BlueprintCallable)
	void NotifyBattleState(EBattleState State);

protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void OnTragetDetected(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void ClearSightEntryInBlackboard();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionComponent* AIPerception;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BT_GoldenSkull;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBlackboardData* BD_GoldenSkull;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BT_KnightSkull;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBlackboardData* BD_KnightSkull;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Perception", meta = (AllowPrivateAccess = "true"))
	float LineOfSightTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Perception", meta = (AllowPrivateAccess = "true"))
	FTimerHandle ClearSightEntryHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Perception", meta = (AllowPrivateAccess = "true"))
	FName HasLineOfSight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Perception", meta = (AllowPrivateAccess = "true"))
	FName EnemyActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception", meta = (AllowPrivateAccess = "true"))
	bool bIsIdentifiedPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception", meta = (AllowPrivateAccess = "true"))
	bool bIsMaintainingDistance;
};

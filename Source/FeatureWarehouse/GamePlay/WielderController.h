// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "WielderController.generated.h"

enum class EBattleState :uint8;

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API AWielderController : public AAIController
{
	GENERATED_BODY()
public:
	AWielderController();

protected:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual void OnPossess(APawn* InPawn) override;

#pragma region Perception
public:
	UFUNCTION()
	void ProcessSight(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void ProcessHearing(AActor* Actor, FAIStimulus Stimulus);

protected:
	/* 타겟을 인지했을 때 실행되는 함수 */
	UFUNCTION() void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionComponent* AIPerception;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception", meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Sight* Sight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception", meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Hearing* Hearing;
#pragma endregion

public:
	UFUNCTION(BlueprintCallable)
	void HandleEnemyState(EStateOfEnemy State, FVector Location = FVector(0.0f, 0.0f, 0.0f), AActor* Target = nullptr);

	UFUNCTION(BlueprintCallable)
	void HandleBattleState(EBattleState State, bool bShouldAttack = false);

	void NotifyUnderAttack(bool IsUnderAttack);

	/* Patrol할 때 실행되는 함수 */
	void NotifyPatrol();

	/* Range 관련 키를 초기화하는 함수 */
	void ClearRangeKey();

	/* 적에게 접근할 때 Behavior Tree에 타겟을 알리는 함수. */
	void NotifyApproachToEnemy(AActor* Enemy);

	/* 무기를 착용했음을 Behavior Tree에 알리는 함수 */
	void NotifyEquipWeapon();

	/* 무장을 해제했음을 Behavior Tree에 알리는 함수 */
	void NotifyUnequip();

	/* 현재 보고있는 Pawn을 반환하는 함수 */
	AActor* GetSeeingPawn();

	/* 플레이어를 식별했는지 */
	bool IsIdentifiedEnemy() { return bIsIdentifiedEnemy; }

	/* MovePos를 블랙보드에 저장하는 함수 */
	void SetMovePosToBlackboard(FVector MovePos);

	/* HomePos를 블랙보드에 저장하는 함수 */
	void SetHomePosToBlackboard(FVector HomePos);

	/* 적 캐릭터를 블랙보드에 저장하는 함수 */
	UFUNCTION(BlueprintCallable)
	void DesignateEnemy(AActor* Enemy);

	/* HomePos로 돌아가는 함수 */
	UFUNCTION(BlueprintCallable) void NotifyGoToHomePos();

	/* 공격받았을 때, 거리를 벌려야 함을 Behavior Tree에 알리는 함수. */
	void NotifyRetreat();

	/* 죽었음을 알리는 함수 */
	void NotifyDead();

	////////////////////////////////////////////////////////////////////    Battle    ////////////////////////////////////////////////////////////////////
	
	/* In-Battle 상태를 BehaviorTree에 알리는 함수 */
	void NotifyBattleState(EBattleState State);

	////////////////////////////////////////////////////////////////////    Battle - Approaching    ////////////////////////////////////////////////////////////////////
	
	/* In-Battle 상태일 때, Behavior Tree에 적에게 다가가는 상태임을 알리는 함수. */
	UFUNCTION(BlueprintCallable) void NotifyApproaching();

	////////////////////////////////////////////////////////////////////    Battle - Attacking    ////////////////////////////////////////////////////////////////////

	/* 적이 공격 범위 내에 있음을 BehaviorTree에 알리는 함수. */
	void NotifyEnemyInAttackRange(bool IsInRange);

	UFUNCTION(BlueprintCallable)
	void FindNewTarget();

	////////////////////////////////////////////////////////////////////    Battle - Monitoring    ////////////////////////////////////////////////////////////////////
	
	/* In-Battle 상태일 때, Behavior Tree에 Monitor 상태에 들어감을 알리는 함수 */
	void NotifyMonitoring();

protected:
	void NotifyEnterIdle();

	void NotifyEngageBattle(AActor* Target);
	
	/* Patrol을 시작하는 함수 */
	void NotifyStartPatrol(FVector Location);

	void NotifyStartFlee();


	void NotifyAttack(bool bShouldAttack);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BT_Wielder;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBlackboardData* BD_Wielder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pawn", meta = (AllowPrivateAccess = "true"))
	class AWielder* Wielder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception", meta = (AllowPrivateAccess = "true"))
	bool bIsIdentifiedEnemy;

	const FName BlackboardEnemyKey = FName("EnemyActor");
	const FName BlackboardHomePosKey = FName("HomePos");
	const FName BlackboardMovePosKey = FName("MovePos");

public:
	FORCEINLINE class UAIPerceptionComponent* GetPerception() { return AIPerception; }
};

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

	void StopBehaviorTree();

	void StartBehaviorTree();

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

#pragma region ProcessState
	////////////////////////////////////////////////////////////////////    Part : Defaults    ////////////////////////////////////////////////////////////////////
public:
	/* 무기를 착용했음을 Behavior Tree에 알리는 함수 */
	void NotifyEquipWeapon();

	/* 무장을 해제했음을 Behavior Tree에 알리는 함수 */
	void NotifyUnequip();
	
	/* 현재 보고있는 Pawn을 반환하는 함수 */
	AActor* GetSeeingPawn();
	
	/* MovePos를 블랙보드에 저장하는 함수 */
	void SetMovePosToBlackboard(FVector MovePos);

	/* HomePos를 블랙보드에 저장하는 함수 */
	void SetHomePosToBlackboard(FVector HomePos);
	
	/* 죽었음을 알리는 함수 */
	void NotifyDead();
	
private:
	const FName BlackboardEnemyKey = FName("EnemyActor");
	const FName BlackboardHomePosKey = FName("HomePos");
	const FName BlackboardMovePosKey = FName("MovePos");

	////////////////////////////////////////////////////////////////////    Part : EnemyState    ////////////////////////////////////////////////////////////////////
public:
	UFUNCTION(BlueprintCallable)
	void HandleEnemyState(EStateOfEnemy State, FVector Location = FVector(0.0f, 0.0f, 0.0f), AActor* Target = nullptr);

	/* Patrol할 때 실행되는 함수 */
	void NotifyPatrol();

	/* HomePos로 돌아가는 함수 */
	UFUNCTION(BlueprintCallable) 
	void NotifyGoToHomePos();
	
protected:
	void ResetPreviousEnemyState(EStateOfEnemy State);

	void NotifyEnterIdle();

	void NotifyEngageBattle(AActor* Target);
	
	/* Patrol을 시작하는 함수 */
	void NotifyStartPatrol();

	void NotifyStartCheck(FVector Location);

	void NotifyStartFlee();

	////////////////////////////////////////////////////////////////////    Part : BattleState    ////////////////////////////////////////////////////////////////////
public:
	
	UFUNCTION(BlueprintCallable)
	void HandleBattleState(EBattleState State, bool bShouldAttack = false);

	/* In-Battle 상태를 Behavior Tree에 알리는 함수 */
	void NotifyBattleState(EBattleState State);
	
	/* 공격받은 상태임을 Behavior Tree에 알리는 함수 */
	void NotifyUnderAttack(bool IsUnderAttack);

	/* 적 캐릭터를 블랙보드에 저장하는 함수 */
	UFUNCTION(BlueprintCallable)
	void DesignateEnemy(AActor* Enemy);
	
	/* In-Battle 상태일 때, Behavior Tree에 적에게 다가가는 상태임을 알리는 함수. */
	UFUNCTION(BlueprintCallable) 
	void NotifyApproaching();
	
	/* 적에게 접근할 때 Behavior Tree에 타겟을 알리는 함수. */
	void NotifyApproachToEnemy(AActor* Enemy);

	/* 공격받았을 때, 거리를 벌려야 함을 Behavior Tree에 알리는 함수. */
	void NotifyRetreat();

	/* In-Battle 상태일 때, Behavior Tree에 Monitor 상태에 들어감을 알리는 함수 */
	void NotifyMonitoring();
	
	/* 적이 공격 범위 내에 있음을 BehaviorTree에 알리는 함수. */
	void NotifyEnemyInAttackRange(bool IsInRange);

protected:
	void ResetPreviousBattleState(EBattleState State);

	////////////////////////////////////////////////////////////////////    Part : Target    ////////////////////////////////////////////////////////////////////
	
public:
	UFUNCTION(BlueprintCallable)
	void FindNewTarget();

	UFUNCTION(BlueprintCallable)
	void ClearTarget();

protected:
	/* Battle 상태 중, 공격 상태임을 Behavior Tree에 알리는 함수  */
	void NotifyAttack(bool bShouldAttack);
	
	// 삭제 고려
	/* Range 관련 키를 초기화하는 함수 */
	void ClearRangeKey();

#pragma endregion ProcessState

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BT_Wielder;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBlackboardData* BD_Wielder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pawn", meta = (AllowPrivateAccess = "true"))
	class AWielder* Wielder;

	

public:
	FORCEINLINE class UAIPerceptionComponent* GetPerception() { return AIPerception; }
};

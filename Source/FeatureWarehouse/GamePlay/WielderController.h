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

	void NotifyUnderAttack(bool IsUnderAttack);

	/* ���𰡸� �������� �� Behavior Tree�� �˸��� �Լ�. */
	void NotifyPerceiveSomething(FVector Location);

	/* Patrol�� �� ����Ǵ� �Լ� */
	void NotifyPatrol();

	/* Range ���� Ű�� �ʱ�ȭ�ϴ� �Լ� */
	void ClearRangeKey();

	/* ���𰡸� �i�ư� �� Behavior Tree�� Ÿ���� �˸��� �Լ�. */
	void NotifyApproachToEnemy(AActor* Enemy);

	/* ���� ��Ȳ�� �������� Behavior Tree�� �˸��� �Լ�. */
	void NotifyEngageInBattle(AActor* Enemy);

	/* ���⸦ ���������� Behavior Tree�� �˸��� �Լ� */
	void NotifyEquipWeapon();

	/* ������ ���������� Behavior Tree�� �˸��� �Լ� */
	void NotifyUnequip();

	/* ���� �����ִ� Pawn�� ��ȯ�ϴ� �Լ� */
	AActor* GetSeeingPawn();

	/* �÷��̾ �ĺ��ߴ��� */
	bool IsIdentifiedEnemy() { return bIsIdentifiedEnemy; }

	/* MovePos�� ������忡 �����ϴ� �Լ� */
	void SetMovePosToBlackboard(FVector MovePos);

	/* HomePos�� ������忡 �����ϴ� �Լ� */
	void SetHomePosToBlackboard(FVector HomePos);

	/* �� ĳ���͸� ������忡 �����ϴ� �Լ� */
	void DesignateEnemy(AActor* Enemy);

	/* HomePos�� ���ư��� �Լ� */
	UFUNCTION(BlueprintCallable) void NotifyGoToHomePos();

	/* ���ݹ޾��� ��, �Ÿ��� ������ ���� Behavior Tree�� �˸��� �Լ�. */
	void NotifyRetreat();

	/* �׾����� �˸��� �Լ� */
	void NotifyDead();

	////////////////////////////////////////////////////////////////////    Battle    ////////////////////////////////////////////////////////////////////
	
	/* In-Battle ���¸� BehaviorTree�� �˸��� �Լ� */
	void NotifyBattleState(EBattleState State);

	////////////////////////////////////////////////////////////////////    Battle - Approaching    ////////////////////////////////////////////////////////////////////
	
	/* In-Battle ������ ��, Behavior Tree�� ������ �ٰ����� �������� �˸��� �Լ�. */
	UFUNCTION(BlueprintCallable) void NotifyApproaching();

	////////////////////////////////////////////////////////////////////    Battle - Attacking    ////////////////////////////////////////////////////////////////////

	/* ���� ���� ���� ���� ������ BehaviorTree�� �˸��� �Լ�. */
	void NotifyEnemyInAttackRange(bool IsInRange);

	UFUNCTION(BlueprintCallable)
	void FindNewTarget();

	////////////////////////////////////////////////////////////////////    Battle - Monitoring    ////////////////////////////////////////////////////////////////////
	
	/* In-Battle ������ ��, Behavior Tree�� Monitor ���¿� ���� �˸��� �Լ� */
	void NotifyMonitoring();

protected:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual void OnPossess(APawn* InPawn) override;

	/* Ÿ���� �������� �� ����Ǵ� �Լ� */
	UFUNCTION() void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionComponent* AIPerception;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BT_Wielder;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBlackboardData* BD_Wielder;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception", meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Sight* Sight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception", meta = (AllowPrivateAccess = "true"))
	bool bIsIdentifiedEnemy;

	const FName BlackboardEnemyKey = FName("EnemyActor");
	const FName BlackboardHomePosKey = FName("HomePos");
	const FName BlackboardMovePosKey = FName("MovePos");

public:
	FORCEINLINE class UAIPerceptionComponent* GetPerception() { return AIPerception; }
};

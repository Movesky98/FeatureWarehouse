// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "WielderController.generated.h"

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API AWielderController : public AAIController
{
	GENERATED_BODY()
public:
	AWielderController();

	/* 무언가를 인지했을 때 Behavior Tree에 알리는 함수. */
	void NotifyPerceiveSomething(FVector Location);

	/* Range 관련 키를 초기화하는 함수 */
	void ClearRangeKey();

	/* 무언가를 쫒아갈 때 Behavior Tree에 타겟을 알리는 함수. */
	void NotifyChaseEnemy(AActor* Enemy);

	/* 전투 상황에 임했음을 Behavior Tree에 알리는 함수. */
	void NotifyEngageInBattle(AActor* Enemy);

	/* 무기를 착용했음을 Behavior Tree에 알리는 함수 */
	void NotifyEquipWeapon();

	/* 무장을 해제했음을 Behavior Tree에 알리는 함수 */
	void NotifyUnequip();

	/* 현재 보고있는 Pawn을 반환하는 함수 */
	AActor* GetSeeingPawn();

	/* 플레이어를 식별했는지 */
	bool IsIdentifiedPlayer() { return bIsIdentifiedPlayer; }

	/* MovePos를 블랙보드에 저장하는 함수 */
	void SetMovePosToBlackboard(FVector MovePos);

	/* HomePos를 블랙보드에 저장하는 함수 */
	void SetHomePosToBlackboard(FVector HomePos);

	/* 적 캐릭터를 블랙보드에 저장하는 함수 */
	void SetEnemyToBlackboard(AActor* Enemy);

protected:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual void OnPossess(APawn* InPawn) override;

	/* 타겟을 인지했을 때 실행되는 함수 */
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
	bool bIsIdentifiedPlayer;

	const FName BlackboardEnemyKey = FName("EnemyActor");
	const FName BlackboardHomePosKey = FName("HomePos");
	const FName BlackboardMovePosKey = FName("MovePos");
};

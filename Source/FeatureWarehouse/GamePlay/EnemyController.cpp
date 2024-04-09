// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "Characters/Enemy.h"
#include "Enums/StateOfEnemy.h"
#include "Enums/BattleState.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"

#include "Perception/AIPerceptionComponent.h"
#include "DrawDebugHelpers.h"

AEnemyController::AEnemyController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AEnemy* Enemy = Cast<AEnemy>(InPawn);

	if (Enemy)
	{
		switch (Enemy->GetEnemyType())
		{
		case ETypeOfEnemy::GoldenSkull:
			if (BT_GoldenSkull && BD_GoldenSkull)
			{
				UBlackboardComponent* BlackboardComp = Blackboard.Get();

				if (UseBlackboard(BD_GoldenSkull, BlackboardComp))
				{
					ensure(RunBehaviorTree(BT_GoldenSkull));

					Blackboard = BlackboardComp;
				}
			}
			break;
		case ETypeOfEnemy::Archer:
		case ETypeOfEnemy::Knight:
			if (BT_KnightSkull && BD_KnightSkull)
			{
				UBlackboardComponent* BlackboardComp = Blackboard.Get();

				if (UseBlackboard(BD_KnightSkull, BlackboardComp))
				{
					ensure(RunBehaviorTree(BT_KnightSkull));

					Blackboard = BlackboardComp;
				}
			}
			break;
		default:
			break;
		}
	}
}

void AEnemyController::BeginPlay()
{
	Super::BeginPlay();

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyController::OnTragetDetected);
}

void AEnemyController::OnTragetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	// 시야에 들어온 액터가 플레이어라면
	if (Actor->ActorHasTag(FName("Player")))
	{
		AEnemy* Enemy = Cast<AEnemy>(GetPawn());

		if (Stimulus.WasSuccessfullySensed())
		{
			bIsIdentifiedPlayer = true;

			// 플레이어가 식별 거리 안에 들어왔다면
			if (Enemy->IsPlayerApproached())
			{
				switch (Enemy->GetEnemyType())
				{
				case ETypeOfEnemy::GoldenSkull:
					NotifyEnemyState(Enemy->IdentifiedPlayer());
					break;
				case ETypeOfEnemy::Knight:
					// 전투 시작.
					Enemy->EngagingInCombat(Actor);
					break;
				default:
					break;
				}
			}
			else
			{
				GetWorldTimerManager().ClearTimer(ClearSightEntryHandle);

				Blackboard->SetValueAsBool(HasLineOfSight, true);
				Blackboard->SetValueAsObject(EnemyActor, Actor);
			}
		}
		else
		{
			bIsIdentifiedPlayer = false;
			
			GetWorldTimerManager().SetTimer(ClearSightEntryHandle, this, &AEnemyController::ClearSightEntryInBlackboard, LineOfSightTime, false);
		}
	}
}

void AEnemyController::ClearSightEntryInBlackboard()
{
	Blackboard->SetValueAsBool(HasLineOfSight, false);
	Blackboard->SetValueAsObject(EnemyActor, nullptr);
}

bool AEnemyController::IsIdentifiedPlayer()
{
	return bIsIdentifiedPlayer;
}

void AEnemyController::NotifyEnemyActor(AActor* Actor)
{
	Blackboard->SetValueAsObject(FName("EnemyActor"), Actor);
}

void AEnemyController::NotifyEnemyInAttackRange(bool IsInAttackRange)
{
	Blackboard->SetValueAsBool(FName("IsEnemyInAttackRange"), IsInAttackRange);
}

void AEnemyController::NotifyEnemyState(EStateOfEnemy State)
{
	uint8 Value = (uint8)State;

	Blackboard->SetValueAsEnum(FName("State"), Value);
}

void AEnemyController::NotifyBattleState(EBattleState State)
{
	uint8 Value = (uint8)State;

	Blackboard->SetValueAsEnum(FName("BattleState"), Value);
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Characters/AICharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"

AEnemyAIController::AEnemyAIController()
{
	
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AAICharacter* AI = Cast<AAICharacter>(InPawn);

	if (AI)
	{
		switch (AI->GetEnemyType())
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
		default:
			break;
		}
	}
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "Characters/Enemy.h"
#include "Enums/StateOfEnemy.h"

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

					GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, Blackboard.GetName());
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

void AEnemyController::BeginPlay()
{
	Super::BeginPlay();

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyController::OnTragetDetected);
}

void AEnemyController::OnTragetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	if (Actor->ActorHasTag(FName("Player")))
	{
		AEnemy* Enemy = Cast<AEnemy>(GetPawn());

		if (Stimulus.WasSuccessfullySensed())
		{
			bIsIdentifiedPlayer = true;

			if (Enemy->IsPlayerApproached())
			{
				NotifyEnemyState(EStateOfEnemy::Flee);
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

void AEnemyController::NotifyEnemyState(EStateOfEnemy State)
{
	uint8 Value = (uint8)State;

	Blackboard->SetValueAsEnum(FName("State"), Value);
}
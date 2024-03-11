// Fill out your copyright notice in the Description page of Project Settings.


#include "WielderController.h"
#include "Characters/Wielder.h"

#include "Enums/StateOfEnemy.h"
#include "Enums/BattleState.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AWielderController::AWielderController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	
	// Create a Sight Sense.
	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	Sight->SightRadius = 1000.0f;
	Sight->LoseSightRadius = 1500.0f;
	Sight->PeripheralVisionAngleDegrees = 80.0f;

	Sight->DetectionByAffiliation.bDetectEnemies = true;
	Sight->DetectionByAffiliation.bDetectFriendlies = true;
	Sight->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerception->ConfigureSense(*Sight);
}

void AWielderController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AWielder* Wielder = Cast<AWielder>(InPawn);

	if (IsValid(Wielder))
	{
		switch (Wielder->GetWielderType())
		{
		case ETypeOfWielder::ETW_Samurai:
			if (BT_Wielder && BD_Wielder)
			{
				UBlackboardComponent* BlackboardComp = Blackboard.Get();

				if (UseBlackboard(BD_Wielder, BlackboardComp))
				{
					ensure(RunBehaviorTree(BT_Wielder));

					Blackboard = BlackboardComp;
				}
			}
			break;

		case ETypeOfWielder::ETW_Halberdier:
			break;

		default:
			break;
		}
	}
}

void AWielderController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AWielderController::OnTargetDetected);
}

void AWielderController::BeginPlay()
{
	Super::BeginPlay();

	// Set Home Position
	Blackboard->SetValueAsVector(BlackboardHomePosKey, GetPawn()->GetActorLocation());
}

void AWielderController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor->ActorHasTag(FName("Player"))) return;
	
	if (Stimulus.WasSuccessfullySensed())
	{
		bIsIdentifiedPlayer = true;

		AWielder* Wielder = Cast<AWielder>(GetPawn());
		if (!IsValid(Wielder)) return;

		// 플레이어가 이미 공격 범위 내에 있을 경우도 만들어야 함. (바로 공격하던가 뒤로 피한 다음 무기를 장착하던가)

		// 플레이어가 이미 접근하였으며, AI가 플레이어를 인식한 경우
		if (Wielder->IsPlayerApproached())
		{
			Wielder->CheckEquipWeapon();
			NotifyEngageInBattle(Actor);
			return;
		}

		// AI가 무언가 접근한 것을 인식했으나, 아직 거리가 있는 경우
		if (Wielder->IsRecognizedSomething())
		{
			// 무언가가 위치한 장소로 정찰
			Wielder->CheckEquipWeapon();
			NotifyPerceiveSomething(Actor->GetActorLocation());
			return;
		}
	}
	else
	{
		// 플레이어가 인식되었다가 시야에서 벗어난 경우.
		bIsIdentifiedPlayer = false;

		// 시야에서 벗어난 경우 처리를 해줘야할 듯.

		// 예를 들어, AI가 무언가를 인식해서 움직이고 있는 도중일 때 플레이어가 시야에서 벗어난 경우라던가
		// 그런 경우엔 AI가 특정 위치로 이동한 후 일정 시간이 지나고나서야 처리하는걸로 할 생각임.
		// 그런 경우 외에도 어떻게 처리할 지 경우의 수를 생각하고 대처를 해야할 듯.
	}
}

void AWielderController::NotifyPerceiveSomething(FVector Location)
{
	AWielder* Wielder = Cast<AWielder>(GetPawn());
	if (IsValid(Wielder))
	{
		Wielder->SetCurState(EStateOfEnemy::Patrol);
		Wielder->SetMovementSpeed(Wielder->CreepSpeed);

		Blackboard->SetValueAsEnum(FName("CurState"), (uint8)Wielder->GetCurState());
		Blackboard->SetValueAsBool(FName("IsRecognizedSomething"), true);
		SetMovePosToBlackboard(Location);
		Blackboard->SetValueAsBool(FName("IsPatrol"), false);
	}
}

void AWielderController::NotifyChaseEnemy(AActor* Enemy)
{
	if (!IsValid(Enemy)) return;

	AWielder* Wielder = Cast<AWielder>(GetPawn());
	if (IsValid(Wielder))
	{
		Wielder->SetCurState(EStateOfEnemy::Chase);
		DesignateEnemy(Enemy);
	}
}

void AWielderController::ClearRangeKey()
{
	Blackboard->SetValueAsBool(FName("IsRecognizedSomething"), false);
	Blackboard->SetValueAsBool(FName("IsPlayerApproached"), false);
	Blackboard->SetValueAsBool(FName("IsInAttackRange"), false);
}

void AWielderController::NotifyRetreat(AActor* Enemy)
{
	AWielder* Wielder = Cast<AWielder>(GetPawn());
	if (IsValid(Wielder))
	{
		Wielder->SetBattleState(EBattleState::Retreating);

		// In_Battle 상태로 들어감.
		if (Wielder->GetCurState() != EStateOfEnemy::In_Battle)
		{
			Wielder->SetCurState(EStateOfEnemy::In_Battle);
			Blackboard->SetValueAsEnum(FName("CurState"), (uint8)Wielder->GetCurState());
		}

		Blackboard->SetValueAsEnum(FName("BattleState"), (uint8)Wielder->CurBattleState());
	}
}

void AWielderController::NotifyEngageInBattle(AActor* Enemy)
{
	AWielder* Wielder = Cast<AWielder>(GetPawn());
	if (IsValid(Wielder))
	{
		Wielder->SetCurState(EStateOfEnemy::In_Battle);
		Wielder->SetMovementSpeed(Wielder->SprintSpeed);

		Blackboard->SetValueAsEnum(FName("CurState"), (uint8)Wielder->GetCurState());
		DesignateEnemy(Enemy);
		NotifyApproaching();
	}
}

void AWielderController::NotifyEquipWeapon()
{
	Blackboard->SetValueAsBool(FName("IsEquipWeapon"), true);
}

void AWielderController::NotifyUnequip()
{
	Blackboard->SetValueAsBool(FName("IsEquipWeapon"), false);
}

void AWielderController::SetMovePosToBlackboard(FVector MovePos)
{
	Blackboard->SetValueAsVector(BlackboardMovePosKey, MovePos);
}

void AWielderController::SetHomePosToBlackboard(FVector HomePos)
{ 
	Blackboard->SetValueAsVector(BlackboardHomePosKey, HomePos); 
}

void AWielderController::DesignateEnemy(AActor* Enemy)
{ 
	Blackboard->SetValueAsObject(BlackboardEnemyKey, Enemy); 
}

void AWielderController::NotifyBattleState(EBattleState State)
{
	Blackboard->SetValueAsEnum(FName("BattleState"), (uint8)State);
}

void AWielderController::NotifyApproaching()
{
	AWielder* Wielder = Cast<AWielder>(GetPawn());
	if (IsValid(Wielder))
	{
		Wielder->SetBattleState(EBattleState::Approaching);
	}

	NotifyBattleState(Wielder->CurBattleState());
}

void AWielderController::NotifyEnemyInAttackRange()
{
	Blackboard->SetValueAsBool(FName("IsInAttackRange"), true);
}

AActor* AWielderController::GetSeeingPawn()
{
	// Return the seeing pawn
	UObject* Object = Blackboard->GetValueAsObject(BlackboardEnemyKey);

	return Object ? Cast<AActor>(Object) : nullptr;
}
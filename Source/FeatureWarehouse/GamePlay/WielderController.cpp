// Fill out your copyright notice in the Description page of Project Settings.


#include "WielderController.h"
#include "Characters/Wielder.h"

#include "Enums/StateOfEnemy.h"

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
	Sight->PeripheralVisionAngleDegrees = 120.0f;

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
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("Binding AIPerception's OnTargetPerceptionUpdated."));
}

void AWielderController::BeginPlay()
{
	Super::BeginPlay();

	// Set Home Position
	Blackboard->SetValueAsVector(BlackboardHomePosKey, GetPawn()->GetActorLocation());
}

void AWielderController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("Something Detected."));
	if (!Actor->ActorHasTag(FName("Player"))) return;
	
	if (Stimulus.WasSuccessfullySensed())
	{
		bIsIdentifiedPlayer = true;

		AWielder* Wielder = Cast<AWielder>(GetPawn());
		if (IsValid(Wielder) && Wielder->IsPlayerApproached())
		{
			// 플레이어가 이미 접근해있고 인식된 경우 전투상황에 들어감.
			NotifyEngageInBattle(Actor);
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
		SetEnemyToBlackboard(Enemy);
	}
}

void AWielderController::ClearRangeKey()
{
	Blackboard->SetValueAsBool(FName("IsRecognizedSomething"), false);
	Blackboard->SetValueAsBool(FName("IsPlayerApproached"), false);
	Blackboard->SetValueAsBool(FName("IsInAttackRange"), false);
}

void AWielderController::NotifyEngageInBattle(AActor* Enemy)
{
	AWielder* Wielder = Cast<AWielder>(GetPawn());
	if (IsValid(Wielder))
	{
		Wielder->SetCurState(EStateOfEnemy::In_Battle);

		Blackboard->SetValueAsEnum(FName("CurState"), (uint8)Wielder->GetCurState());
		SetEnemyToBlackboard(Enemy);
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

void AWielderController::SetEnemyToBlackboard(AActor* Enemy)
{ 
	Blackboard->SetValueAsObject(BlackboardEnemyKey, Enemy); 
}

AActor* AWielderController::GetSeeingPawn()
{
	// Return the seeing pawn
	UObject* Object = Blackboard->GetValueAsObject(BlackboardEnemyKey);

	return Object ? Cast<AActor>(Object) : nullptr;
}
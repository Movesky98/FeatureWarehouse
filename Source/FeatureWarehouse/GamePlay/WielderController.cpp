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
			// �÷��̾ �̹� �������ְ� �νĵ� ��� ������Ȳ�� ��.
			NotifyEngageInBattle(Actor);
		}
	}
	else
	{
		// �÷��̾ �νĵǾ��ٰ� �þ߿��� ��� ���.
		bIsIdentifiedPlayer = false;

		// �þ߿��� ��� ��� ó���� ������� ��.

		// ���� ���, AI�� ���𰡸� �ν��ؼ� �����̰� �ִ� ������ �� �÷��̾ �þ߿��� ��� �������
		// �׷� ��쿣 AI�� Ư�� ��ġ�� �̵��� �� ���� �ð��� ���������� ó���ϴ°ɷ� �� ������.
		// �׷� ��� �ܿ��� ��� ó���� �� ����� ���� �����ϰ� ��ó�� �ؾ��� ��.
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
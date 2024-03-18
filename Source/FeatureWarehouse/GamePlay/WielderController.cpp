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

	// Set AIPerception's Sight Radius.
	FAISenseID Id = UAISense::GetSenseID(UAISense_Sight::StaticClass());

	if (!Id.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Wrong Sense ID"));
		return;
	}

	auto Config = AIPerception->GetSenseConfig(Id);

	if (Config == nullptr)
		return;

	auto ConfigSight = Cast<UAISenseConfig_Sight>(Config);

	// Save original lose range
	float LoseRange = ConfigSight->LoseSightRadius - ConfigSight->SightRadius;

	ConfigSight->SightRadius = Wielder->GetRecognizeRange();

	// Apply lose range to new radius of the sight
	ConfigSight->LoseSightRadius = ConfigSight->SightRadius + LoseRange;

	AIPerception->RequestStimuliListenerUpdate();

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

		// �̹� ���� ���� ��� ����
		if (Wielder->GetCurState() == EStateOfEnemy::In_Battle && Wielder->GetSeeingPawn())	return;

		// �÷��̾ �̹� �����Ͽ�����, AI�� �÷��̾ �ν��� ���
		if (Wielder->IsPlayerApproached())
		{
			Wielder->CheckEquipWeapon();
			Wielder->ShowStatBar();
			NotifyEngageInBattle(Actor);
			return;
		}

		// AI�� ���� ������ ���� �ν�������, ���� �Ÿ��� �ִ� ���
		if (Wielder->IsRecognizedSomething())
		{
			// ���𰡰� ��ġ�� ��ҷ� ����
			Wielder->CheckEquipWeapon();
			Wielder->ShowStatBar();
			NotifyPerceiveSomething(Actor->GetActorLocation());
			return;
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

void AWielderController::NotifyApproachToEnemy(AActor* Enemy)
{
	if (!IsValid(Enemy)) return;

	AWielder* Wielder = Cast<AWielder>(GetPawn());
	if (IsValid(Wielder))
	{
		Wielder->SetBattleState(EBattleState::Approaching);

		if (Wielder->GetCurState() != EStateOfEnemy::In_Battle)
		{
			Wielder->SetCurState(EStateOfEnemy::In_Battle);
			Blackboard->SetValueAsEnum(FName("CurState"), (uint8)Wielder->CurBattleState());
		}

		Blackboard->SetValueAsEnum(FName("BattleState"), (uint8)Wielder->CurBattleState());
		DesignateEnemy(Enemy);
	}
}

void AWielderController::ClearRangeKey()
{
	Blackboard->SetValueAsBool(FName("IsRecognizedSomething"), false);
	Blackboard->SetValueAsBool(FName("IsPlayerApproached"), false);
	Blackboard->SetValueAsBool(FName("IsInAttackRange"), false);
}

void AWielderController::NotifyRetreat()
{
	AWielder* Wielder = Cast<AWielder>(GetPawn());
	if (IsValid(Wielder))
	{
		Wielder->SetBattleState(EBattleState::Retreating);

		// In_Battle ���·� ��.
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

		// ���� �Ÿ� �̻��̸� ����͸�, �ƴϸ� ����.
		Wielder->GetDistanceTo(Enemy) > 600.0f ? Wielder->Monitoring() : NotifyApproaching();
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
	if (GetSeeingPawn() != Enemy)
	{
		Blackboard->SetValueAsObject(BlackboardEnemyKey, Enemy); 
	}
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
		Wielder->SetMovementSpeed(Wielder->SprintSpeed);

		NotifyBattleState(Wielder->CurBattleState());
	}
}

void AWielderController::NotifyMonitoring()
{
	AWielder* Wielder = Cast<AWielder>(GetPawn());
	if (IsValid(Wielder))
	{
		Wielder->SetBattleState(EBattleState::Monitoring);
		Wielder->SetMovementSpeed(Wielder->CreepSpeed);

		NotifyBattleState(Wielder->CurBattleState());
	}
}

void AWielderController::NotifyEnemyInAttackRange()
{
	Blackboard->SetValueAsBool(FName("IsInAttackRange"), true);
}

void AWielderController::NotifyDead()
{
	Blackboard->SetValueAsBool(FName("IsDead"), true);
}

AActor* AWielderController::GetSeeingPawn()
{
	// Return the seeing pawn
	UObject* Object = Blackboard->GetValueAsObject(BlackboardEnemyKey);

	return Object ? Cast<AActor>(Object) : nullptr;
}
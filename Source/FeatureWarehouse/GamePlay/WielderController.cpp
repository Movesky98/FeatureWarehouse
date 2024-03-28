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
#include "Kismet/KismetSystemLibrary.h"

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
		if (BT_Wielder && BD_Wielder)
		{
			UBlackboardComponent* BlackboardComp = Blackboard.Get();

			if (UseBlackboard(BD_Wielder, BlackboardComp))
			{
				ensure(RunBehaviorTree(BT_Wielder));

				Blackboard = BlackboardComp;
			}
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
	UE_LOG(LogTemp, Warning, TEXT("%s is Detected Target (%s)"), *UKismetSystemLibrary::GetDisplayName(GetPawn()), *UKismetSystemLibrary::GetDisplayName(Actor));

	if (GetSeeingPawn() != nullptr) return;
	
	AWielder* Wielder = Cast<AWielder>(GetPawn());
	if (!IsValid(Wielder)) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		bIsIdentifiedEnemy = true;

		// 이미 전투 중일 경우 아무것도 하지 않음.
		if (Wielder->GetCurState() == EStateOfEnemy::In_Battle) return;

		// 적이 이미 접근하였으며, AI가 적을 인식한 경우
		if (Wielder->IsEnemyApproached())
		{
			Wielder->CheckEquipWeapon();
			Wielder->ShowStatBar();
			NotifyEngageInBattle(Actor);
			return;
		}

		// AI가 무언가 접근한 것을 인식했으나, 아직 거리가 있는 경우
		if (Wielder->IsRecognizedSomething())
		{
			// 무언가가 위치한 장소로 정찰
			Wielder->CheckEquipWeapon();
			Wielder->ShowStatBar();
			NotifyPerceiveSomething(Actor->GetActorLocation());
			return;
		}
	}
	else
	{
		// 플레이어가 인식되었다가 시야에서 벗어난 경우.
		bIsIdentifiedEnemy = false;

		// 시야에서 벗어난 경우 처리를 해줘야할 듯.
		if (!Wielder->IsRecognizedSomething())
		{
			NotifyGoToHomePos();
		}
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
		Blackboard->SetValueAsBool(FName("IsDetection"), true);
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
	Blackboard->SetValueAsBool(FName("IsEnemyApproached"), false);
	Blackboard->SetValueAsBool(FName("IsInAttackRange"), false);
}

void AWielderController::NotifyRetreat()
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

		// 일정 거리 이상이면 모니터링, 아니면 접근.
		Wielder->GetDistanceTo(Enemy) > 600.0f ? Wielder->Monitoring() : NotifyApproaching();
	}
}

void AWielderController::NotifyPatrol()
{
	AWielder* Wielder = Cast<AWielder>(GetPawn());
	if (IsValid(Wielder))
	{
		Wielder->SetCurState(EStateOfEnemy::Patrol);
		Wielder->SetMovementSpeed(Wielder->WalkSpeed);

		Blackboard->SetValueAsEnum(FName("CurState"), (uint8)Wielder->GetCurState());
		Blackboard->SetValueAsBool(FName("IsDetection"), false);
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

void AWielderController::NotifyEnemyInAttackRange(bool IsInRange)
{
	UE_LOG(LogTemp, Warning, TEXT("%s Change Behavior Tree's AttackRange State."), *UKismetSystemLibrary::GetDisplayName(GetPawn()));

	Blackboard->SetValueAsBool(FName("IsInAttackRange"), IsInRange);
}

void AWielderController::NotifyGoToHomePos()
{
	AWielder* Wielder = Cast<AWielder>(GetPawn());
	if (IsValid(Wielder))
	{
		Wielder->SetCurState(EStateOfEnemy::Idle);
		Wielder->SetMovementSpeed(Wielder->WalkSpeed);
		
		// Need to Clear In_Battle State.

		Blackboard->SetValueAsEnum(FName("CurState"), (uint8)Wielder->GetCurState());
	}
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
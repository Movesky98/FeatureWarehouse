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
#include "Perception/AISenseConfig_Hearing.h"
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

	Hearing = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	Hearing->HearingRange = 2500.0f;
	Hearing->DetectionByAffiliation.bDetectEnemies = true;
	Hearing->DetectionByAffiliation.bDetectFriendlies = false;
	Hearing->DetectionByAffiliation.bDetectNeutrals = false;

	AIPerception->ConfigureSense(*Sight);
	AIPerception->ConfigureSense(*Hearing);
}

void AWielderController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AWielder* WielderPawn = Cast<AWielder>(InPawn);
	
	if (IsValid(WielderPawn))
	{
		Wielder = WielderPawn;

		if (BT_Wielder && BD_Wielder)
		{
			UBlackboardComponent* BlackboardComp = Blackboard.Get();

			if (UseBlackboard(BD_Wielder, BlackboardComp))
			{
				ensure(RunBehaviorTree(BT_Wielder));

				Blackboard = BlackboardComp;

				// Set Home Position
				Blackboard->SetValueAsVector(BlackboardHomePosKey, GetPawn()->GetActorLocation());
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

	ConfigSight->SightRadius = Wielder->GetUncertainDetectionRange();

	// Apply lose range to new radius of the sight
	ConfigSight->LoseSightRadius = ConfigSight->SightRadius + LoseRange;

	AIPerception->RequestStimuliListenerUpdate();
	UE_LOG(LogTemp, Warning, TEXT("Set Sense ID"));
}

void AWielderController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AWielderController::OnTargetDetected);
}

void AWielderController::BeginPlay()
{
	Super::BeginPlay();

}

#pragma region Perception
void AWielderController::ProcessSight(AActor* Actor, FAIStimulus Stimulus)
{
	if (!IsValid(Wielder)) return;

	AWielderBase* WielderBase = Cast<AWielderBase>(Actor);
	if (!IsValid(WielderBase)) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		// 식별한 Wielder가 리스트에 없다면 추가
		if (!Wielder->FindPerceivedWielder(WielderBase))
		{
			Wielder->UpdatePerceivedWielders(WielderBase);
		}

		// 현재 바라보고 있는 폰이 있거나, 같은 팀이라면 아무것도 하지 않음.
		if (GetSeeingPawn()) return;
		if (WielderBase->GetGenericTeamId() == Wielder->GetGenericTeamId()) return;

		EPerceptionState* PerceptionState = Wielder->GetInRangeWielders().Find(WielderBase);

		// 적이 이미 접근하였으며, AI가 적을 인식한 경우
		switch (*PerceptionState)
		{
		case EPerceptionState::EPS_InDetectedRange:
			Wielder->CheckEquipWeapon();
			Wielder->ShowStatBar();
			HandleEnemyState(EStateOfEnemy::In_Battle, FVector(0.0f), Actor);
			break;
		case EPerceptionState::EPS_InRecognizedRange:
			Wielder->CheckEquipWeapon();
			Wielder->ShowStatBar();
			HandleEnemyState(EStateOfEnemy::Patrol, Actor->GetActorLocation());
			break;
		default:
			break;
		}
	}
	else
	{
		// 플레이어가 인식되었다가 시야에서 벗어난 경우.
		bIsIdentifiedEnemy = false;

		// 현재 바라보고 있는 폰이 아니면 아무것도 안함.
		if (GetSeeingPawn() != Actor) return;

		/*EPerceptionState* PerceptionState = Wielder->GetInRangeWielders().Find(WielderBase);

		if (Wielder->GetInRangeWielders().Num() > 0)
		{
			FindNewTarget();
		}*/

		// 시야에서 벗어난 경우 처리를 해줘야할 듯.
		//if (!Wielder->IsRecognizedSomething())
		//{
		//	// NotifyGoToHomePos();
		//}
	}
}

void AWielderController::ProcessHearing(AActor* Actor, FAIStimulus Stimulus)
{
	if (!IsValid(Wielder) || GetSeeingPawn()) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		Wielder->CheckEquipWeapon();
		NotifyStartPatrol(Actor->GetActorLocation());
	}
}

void AWielderController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{	
	const UAISenseConfig* AISenseConfig =  AIPerception->GetSenseConfig(Stimulus.Type);

	FString SenseType = AISenseConfig->GetSenseName();

	if (SenseType == TEXT("Sight"))
	{
		ProcessSight(Actor, Stimulus);
	}
	else if (SenseType == TEXT("Hearing"))
	{
		ProcessHearing(Actor, Stimulus);
	}
}

#pragma endregion

#pragma region DefaultsStates
void AWielderController::HandleEnemyState(EStateOfEnemy State, FVector Location, AActor* Target)
{
	if (!IsValid(Wielder)) return;
	
	Wielder->SetCurState(State);

	switch (State)
	{
	case EStateOfEnemy::Idle:
		NotifyEnterIdle();
		break;
	case EStateOfEnemy::Patrol:
		NotifyStartPatrol(Location);
		break;
	case EStateOfEnemy::In_Battle:
		NotifyEngageBattle(Target);
		break;
	case EStateOfEnemy::Flee:
		// DO NOTHING...
		break;
	default:
		break;
	}
}

void AWielderController::NotifyEnterIdle()
{
	
}

void AWielderController::NotifyEngageBattle(AActor* Target)
{
	Wielder->SetMovementSpeed(Wielder->SprintSpeed);

	Blackboard->SetValueAsEnum(FName("CurState"), (uint8)Wielder->GetCurState());
	DesignateEnemy(Target);

	// 일정 거리 이상이면 모니터링, 아니면 접근.
	Wielder->GetDistanceTo(Target) > 600.0f ? Wielder->Monitoring() : NotifyApproaching();
}

void AWielderController::NotifyStartPatrol(FVector Location)
{
	Wielder->SetMovementSpeed(Wielder->CreepSpeed);

	Blackboard->SetValueAsEnum(FName("CurState"), (uint8)Wielder->GetCurState());
	Blackboard->SetValueAsBool(FName("IsRecognizedSomething"), true);
	SetMovePosToBlackboard(Location);
	Blackboard->SetValueAsBool(FName("IsDetection"), true);
}

void AWielderController::NotifyStartFlee()
{

}
#pragma endregion

#pragma region BattleStates
void AWielderController::HandleBattleState(EBattleState State, bool bShouldAttack)
{
	Wielder->SetBattleState(State);

	switch (State)
	{
	case EBattleState::Monitoring:
		NotifyMonitoring();
		break;
	case EBattleState::Approaching:
		NotifyApproaching();
		break;
	case EBattleState::Attacking:
		NotifyAttack(bShouldAttack);
		break;
	case EBattleState::Retreating:
		NotifyRetreat();
		break;
	default:
		break;
	}
}

void AWielderController::NotifyApproachToEnemy(AActor* Enemy)
{
	if (IsValid(Enemy) && IsValid(Wielder)) return;
	
	Wielder->SetBattleState(EBattleState::Approaching);

	if (Wielder->GetCurState() != EStateOfEnemy::In_Battle)
	{
		Wielder->SetCurState(EStateOfEnemy::In_Battle);
		Blackboard->SetValueAsEnum(FName("CurState"), (uint8)Wielder->CurBattleState());
	}

	Blackboard->SetValueAsEnum(FName("BattleState"), (uint8)Wielder->CurBattleState());
	DesignateEnemy(Enemy);
}

void AWielderController::ClearRangeKey()
{
	Blackboard->SetValueAsBool(FName("IsRecognizedSomething"), false);
	Blackboard->SetValueAsBool(FName("IsEnemyApproached"), false);
	Blackboard->SetValueAsBool(FName("IsInAttackRange"), false);
}

void AWielderController::NotifyRetreat()
{
	if (!IsValid(Wielder)) return;
	
	Wielder->SetBattleState(EBattleState::Retreating);

	// In_Battle 상태로 들어감.
	if (Wielder->GetCurState() != EStateOfEnemy::In_Battle)
	{
		Wielder->SetCurState(EStateOfEnemy::In_Battle);
		Blackboard->SetValueAsEnum(FName("CurState"), (uint8)Wielder->GetCurState());
	}

	Blackboard->SetValueAsEnum(FName("BattleState"), (uint8)Wielder->CurBattleState());
}

void AWielderController::NotifyPatrol()
{
	if (!IsValid(Wielder)) return;
	
	Wielder->SetCurState(EStateOfEnemy::Patrol);
	Wielder->SetMovementSpeed(Wielder->WalkSpeed);

	Blackboard->SetValueAsEnum(FName("CurState"), (uint8)Wielder->GetCurState());
	Blackboard->SetValueAsBool(FName("IsDetection"), false);
}

void AWielderController::NotifyAttack(bool bShouldAttack)
{
	NotifyBattleState(Wielder->CurBattleState());

	Blackboard->SetValueAsBool(FName("IsInAttackRange"), bShouldAttack);
}

#pragma endregion

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
	if (!IsValid(Wielder)) return;

	Wielder->SetBattleState(EBattleState::Approaching);
	Wielder->SetMovementSpeed(Wielder->SprintSpeed);

	NotifyBattleState(Wielder->CurBattleState());
}

void AWielderController::NotifyMonitoring()
{
	if (!IsValid(Wielder)) return;
	
	Wielder->SetBattleState(EBattleState::Monitoring);
	Wielder->SetMovementSpeed(Wielder->CreepSpeed);

	NotifyBattleState(Wielder->CurBattleState());
}


void AWielderController::NotifyEnemyInAttackRange(bool IsInRange)
{
	Blackboard->SetValueAsBool(FName("IsInAttackRange"), IsInRange);
}

void AWielderController::NotifyGoToHomePos()
{
	if (!IsValid(Wielder)) return;

	Wielder->SetCurState(EStateOfEnemy::Idle);
	Wielder->SetMovementSpeed(Wielder->WalkSpeed);
		
	// Need to Clear In_Battle State.
	Blackboard->SetValueAsEnum(FName("CurState"), (uint8)Wielder->GetCurState());
}

void AWielderController::NotifyUnderAttack(bool IsUnderAttack)
{
	Blackboard->SetValueAsBool(FName("IsUnderAttack"), IsUnderAttack);
}

void AWielderController::FindNewTarget()
{
	if (!GetSeeingPawn()) return;


	/*TArray<AWielderBase*> DetectedWielders;
	DetectedWielders = Wielder->GetInRangeWielders();

	for (auto DetectedWielder : DetectedWielders)
	{
		if (IsValid(DetectedWielder))
		{
			if (DetectedWielder->GetGenericTeamId() == Wielder->GetGenericTeamId())
				continue;

			Wielder->GetDistanceTo(DetectedWielder) <= Wielder->GetAttackRange() ? NotifyEnemyInAttackRange(true) : NotifyEnemyInAttackRange(false);
			DesignateEnemy(DetectedWielder);
			return;
		}
	}*/

	// NotifyGoToHomePos();
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
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

}

void AWielderController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{	
	AWielder* Wielder = Cast<AWielder>(GetPawn());
	if (!IsValid(Wielder)) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		bIsIdentifiedEnemy = true;

		// ������ Wielder�� ����
		AWeaponWielder* WeaponWielder = Cast<AWeaponWielder>(Actor);
		if (WeaponWielder)
		{
			Wielder->AddDetectedWielder(WeaponWielder);
			UE_LOG(LogTemp, Warning, TEXT("%s Add Detected Wielder %s"), *UKismetSystemLibrary::GetDisplayName(Wielder), *UKismetSystemLibrary::GetDisplayName(WeaponWielder));
		}

		// ���� �ٶ󺸰� �ִ� ���� ������ �ƹ��͵� ��������.
		if (GetSeeingPawn()) return;

		// ���� �̹� �����Ͽ�����, AI�� ���� �ν��� ���
		if (Wielder->IsEnemyApproached())
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
		bIsIdentifiedEnemy = false;

		// ���� �ٶ󺸰� �ִ� ���� �ƴϸ� �ƹ��͵� ����.
		if (GetSeeingPawn() != Actor) return;

		if (Wielder->GetDetectedWielders().Num() > 0)
		{
			FindNewTarget();
		}

		// �þ߿��� ��� ��� ó���� ������� ��.
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

void AWielderController::NotifyUnderAttack(bool IsUnderAttack)
{
	Blackboard->SetValueAsBool(FName("IsUnderAttack"), IsUnderAttack);
}

void AWielderController::FindNewTarget()
{
	AWielder* Wielder = Cast<AWielder>(GetPawn());
	if (!IsValid(Wielder)) return;

	TArray<AWeaponWielder*> DetectedWielders;
	DetectedWielders = Wielder->GetDetectedWielders();

	for (auto DetectedWielder : DetectedWielders)
	{
		if (IsValid(DetectedWielder))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s Set New Target"), *UKismetSystemLibrary::GetDisplayName(Wielder));

			Wielder->GetDistanceTo(DetectedWielder) <= Wielder->GetAttackRange() ? NotifyEnemyInAttackRange(true) : NotifyEnemyInAttackRange(false);
			DesignateEnemy(DetectedWielder);
			return;
		}
	}

	NotifyGoToHomePos();
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
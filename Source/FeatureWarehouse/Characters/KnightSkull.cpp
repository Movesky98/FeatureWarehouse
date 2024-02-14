// Fill out your copyright notice in the Description page of Project Settings.

#include "KnightSkull.h"
#include "GamePlay/EnemyController.h"

#include "Enums/StateOfEnemy.h"
#include "Enums/BattleState.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Math/RandomStream.h"
#include "DrawDebugHelpers.h"

AKnightSkull::AKnightSkull()
{
	Sword = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Sword"));
	Sword->SetupAttachment(GetMesh(), FName("SwordSocket"));
	Sword->SetCollisionProfileName(FName("NoCollision"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Knight(TEXT("/Game/ImportAssets/CityofBrass_Enemies/Meshes/Enemy/Corpse_Basic/Corpse_Basic"));
	if (SK_Knight.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Knight.Object);
	}
}

void AKnightSkull::BeginPlay()
{
	Super::BeginPlay();

	GetTriggerZone()->SetSphereRadius(MonitoringDistance);
	GetTriggerZone()->OnComponentBeginOverlap.AddDynamic(this, &AKnightSkull::OnTriggerBeginOverlap);
	GetTriggerZone()->OnComponentEndOverlap.AddDynamic(this, &AKnightSkull::OnTriggerEndOverlap);
}

void AKnightSkull::OnTriggerBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		bIsPlayerApproached = true;

		AEnemyController* EnemyController = Cast<AEnemyController>(GetController());
		if (EnemyController->IsIdentifiedPlayer())
		{
			// 전투 시작.
			EngagingInCombat(OtherActor);
		}
	}
}

void AKnightSkull::OnTriggerEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		bIsPlayerApproached = false;
	}
}

void AKnightSkull::OnAttackRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		bIsPlayerInAttackRange = true;

		AEnemyController* EnemyController = Cast<AEnemyController>(GetController());
		if (EnemyController)
		{
			EnemyController->NotifyEnemyInAttackRange(bIsPlayerInAttackRange);
			EnemyController->NotifyBattleState(EBattleState::Attacking);
		}
	}
}

void AKnightSkull::OnAttackRangeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		bIsPlayerInAttackRange = false;

		AEnemyController* EnemyController = Cast<AEnemyController>(GetController());
		if (EnemyController)
		{
			EnemyController->NotifyEnemyInAttackRange(bIsPlayerInAttackRange);
			EnemyController->NotifyBattleState(EBattleState::Approaching);
		}
	}
}


void AKnightSkull::EngagingInCombat(AActor* AdversaryActor)
{
	AEnemyController* EnemyController = Cast<AEnemyController>(GetController());
	
	if (!ensure(EnemyController != nullptr)) return;

	EnemyController->NotifyEnemyActor(AdversaryActor);
	EnemyController->NotifyEnemyState(EStateOfEnemy::In_Battle);

	// 이 부분은 나중에 공격, 뒤로 물리기 기능이 완성되면 셋 중 랜덤으로 작동하도록 해도 괜찮을 듯.
	EnemyController->NotifyBattleState(EBattleState::Monitoring);
	EnemyController->SetFocus(AdversaryActor);
}

void AKnightSkull::MonitoringPlayer()
{
	/*
	* 이 함수에서 플레이어를 경계하는 기능 구현
	* 1. 위치에 가만히 서서 플레이어를 바라보던가.
	* 2. 플레이어를 기준으로 원형으로 돌던가.
	*/
	bIsStartMonitoring = true;
	bIsCirclingAroundThePlayer = UKismetMathLibrary::RandomBool();

	if (bIsCirclingAroundThePlayer)
	{
		DrawDebugString(GetWorld(), FVector(0.0f, 0.0f, 150.0f), FString("KnightSkull :: Circling around the player !"), this, FColor::Green, 4.0f);
	}
	else
	{
		DrawDebugString(GetWorld(), FVector(0.0f, 0.0f, 150.0f), FString("KnightSkull :: Maintaining distance from the player !"), this, FColor::Green, 4.0f);
	}

	MonitoringTimerDuration = 5.0f + UKismetMathLibrary::RandomFloatInRange(0.0f, 3.0f);
	GetWorldTimerManager().SetTimer(MonitoringTimer, this, &AKnightSkull::CalculateMonitoringTime, 0.01f, true);
}

void AKnightSkull::CalculateMonitoringTime()
{
	if (MonitoringTimerDuration < ElapsedTime)
	{
		GetWorldTimerManager().ClearTimer(MonitoringTimer);
		ElapsedTime = 0.0f;
		MonitoringTimerDuration = 0.0f;

		// Change Battle State.
		AEnemyController* EnemyController = Cast<AEnemyController>(GetController());
		if (EnemyController)
		{
			EnemyController->NotifyBattleState(EBattleState::Approaching);
		}
	}
	else
	{
		ElapsedTime += 0.01f;
	}
}

FVector AKnightSkull::MaintainingDistance()
{
	FVector MoveLocation;
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if(!ensure(Player != nullptr)) return GetActorLocation();
	
	FVector Direction = GetActorLocation() - Player->GetActorLocation();
	Direction = Direction.GetSafeNormal();

	if (GetDistanceTo(Player) < MonitoringDistance)
	{
		MoveLocation = GetActorLocation() + Direction * (MonitoringDistance - GetDistanceTo(Player));
			
		UWorld* World = GetWorld();
		if (!ensure(World != nullptr)) return GetActorLocation();
		
		TArray<AActor*> IgnoreActors;
		FHitResult Hit;

		bool IsSuccess = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			MoveLocation,
			MoveLocation - FVector(0.0f, 0.0f, 1000.0f),
			ETraceTypeQuery::TraceTypeQuery1,
			false,
			IgnoreActors,
			EDrawDebugTrace::ForDuration,
			Hit,
			true
			);
		
		if (IsSuccess)
		{
			return Hit.ImpactPoint;
		}

		return MoveLocation;
	}
	

	return GetActorLocation();
}

FVector AKnightSkull::CirclingAroundThePlayer()
{
	FVector MoveLocation;

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!ensure(Player != nullptr)) return GetActorLocation();
	
	FVector DirectionFromPlayer = GetActorLocation() - Player->GetActorLocation();
	DirectionFromPlayer = DirectionFromPlayer.GetSafeNormal();

	FRotator DirectionRot = UKismetMathLibrary::MakeRotFromX(DirectionFromPlayer);
	DirectionRot.Yaw -= 0.5f;	// 일단 왼쪽으로 이동하게 해봄.
		
	FVector GoalDirection = UKismetMathLibrary::GetForwardVector(DirectionRot);

	if (GetDistanceTo(Player) < MonitoringDistance)
	{
		MoveLocation = GetActorLocation() + GoalDirection * (MonitoringDistance - GetDistanceTo(Player));
		
		UWorld* World = GetWorld();
		if (!ensure(World != nullptr)) return GetActorLocation();

		TArray<AActor*> IgnoreActors;
		FHitResult Hit;

		bool IsSuccess = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			MoveLocation,
			MoveLocation - FVector(0.0f, 0.0f, 1000.0f),
			ETraceTypeQuery::TraceTypeQuery1,
			false,
			IgnoreActors,
			EDrawDebugTrace::ForDuration,
			Hit,
			true
		);

		if (IsSuccess)
		{
			return Hit.ImpactPoint;
		}

		return MoveLocation;
	}
	

	return GetActorLocation();
}
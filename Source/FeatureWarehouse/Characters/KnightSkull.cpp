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
			EnemyController->NotifyEnemyActor(OtherActor);
			EnemyController->NotifyEnemyState(EStateOfEnemy::In_Battle);
			EnemyController->NotifyBattleState(EBattleState::Monitoring);
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

void AKnightSkull::MonitoringPlayer()
{
	/*
	* 이 함수에서 플레이어를 경계하는 기능 구현
	* 1. 위치에 가만히 서서 플레이어를 바라보던가.
	* 2. 플레이어를 기준으로 원형으로 돌던가.
	*/
	bIsStartMonitoring = true;
	bIsCirclingAroundThePlayer = UKismetMathLibrary::RandomBool();

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
	/*
	* 여기선 일정 시간동안 플레이어를 경계하기 위해 플레이어를 바라보면서 거리를 유지해야함.
	* 따라서, 플레이어의 위치를 따온 다음 이 캐릭터와의 방향을 구해 플레이어가 다가오면 뒤로 가도록 구현할 것임.
	*/

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
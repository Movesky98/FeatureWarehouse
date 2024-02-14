// Fill out your copyright notice in the Description page of Project Settings.


#include "GoldenSkull.h"
#include "Enums/StateOfEnemy.h"
#include "GamePlay/EnemyController.h"

#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GamePlayStatics.h"
#include "DrawDebugHelpers.h"

AGoldenSkull::AGoldenSkull()
{

}

void  AGoldenSkull::BeginPlay()
{
	Super::BeginPlay();

	SetMovemenetSpeed(NormalSpeed);

	GetTriggerZone()->OnComponentBeginOverlap.AddDynamic(this, &AGoldenSkull::OnTriggerBeginOverlap);
	GetTriggerZone()->OnComponentEndOverlap.AddDynamic(this, &AGoldenSkull::OnTriggerEndOverlap);
}

void AGoldenSkull::OnTriggerBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		bIsPlayerApproached = true;

		AEnemyController* EnemyController = Cast<AEnemyController>(GetController());
		if (EnemyController->IsIdentifiedPlayer())
		{
			IdentifiedPlayer();
			EnemyController->NotifyEnemyState(EStateOfEnemy::Flee);
		}
	}
}

void AGoldenSkull::OnTriggerEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		bIsPlayerApproached = false;
	}
}

EStateOfEnemy AGoldenSkull::IdentifiedPlayer()
{
	SetMovemenetSpeed(FleeSpeed);

	return EStateOfEnemy::Flee;
}

FVector AGoldenSkull::CalculateFleeLocation()
{
	UWorld* World = GetWorld();

	if (World)
	{
		// (180° ÷ 쏠 Trace의 개수) / 일정한 각도로 레이를 쏘기 위해 계산.
		float DeltaYaw = 180.0f / (FleeTraceNum - 1);

		// Player's location - GoldenSkull's location == Goal Direction. 
		FVector FleeDirection = GetActorLocation();
		FleeDirection -= UGameplayStatics::GetPlayerCharacter(World, 0)->GetActorLocation();

		FRotator FirstTraceStartRot = UKismetMathLibrary::MakeRotFromX(FleeDirection.GetSafeNormal());
		FirstTraceStartRot.Roll = 0.0f;
		FirstTraceStartRot.Pitch = 0.0f;
		FirstTraceStartRot.Yaw -= 90.0f;

		float ReturnDistance = 0.0f;
		FVector ReturnLocation = FVector::ZeroVector;
		
		FVector Direction;
		TArray<AActor*> IgnoreActors;
		FHitResult Hit;

		for (int i = 0; i < FleeTraceNum; i++)
		{
			Direction = UKismetMathLibrary::GetForwardVector(CalculateTraceRotation(FirstTraceStartRot, DeltaYaw, i));

			bool IsHit = UKismetSystemLibrary::CapsuleTraceSingle(
				World,
				GetActorLocation(),
				Direction * MaxAvoidanceRange + GetActorLocation(),
				GetCapsuleComponent()->GetUnscaledCapsuleRadius(),
				GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(),
				ETraceTypeQuery::TraceTypeQuery1,
				false,
				IgnoreActors,
				EDrawDebugTrace::ForDuration,
				Hit,
				true
			);

			// 맞은 위치, 거리에 따라 어느 지점을 선택할지 결정.
			if (IsHit)
			{
				if (Hit.Distance < MinAvoidanceRange) continue;

				if (ReturnDistance < Hit.Distance)
				{
					ReturnDistance = Hit.Distance;
					ReturnLocation = GetFloorLocationOnHit(Hit.Location);
				}
			}
			else
			{
				if (ReturnDistance <= MaxAvoidanceRange)
				{
					ReturnDistance = MaxAvoidanceRange;
					ReturnLocation = GetFloorLocationOnHit(Direction * MaxAvoidanceRange + GetActorLocation());
					break;
				}
			}
		}

		// Step 3. 계산된 거리 중 가장 긴 거리의 바닥 위치를 그림.
		DrawDebugSolidBox(World, ReturnLocation, FVector(10.0f), FColor::Cyan, false, 5.0f, 5);
		return ReturnLocation;
	}

	return FVector(0.0f);
}

FRotator AGoldenSkull::CalculateTraceRotation(const FRotator& FirstRotation, const float& DeltaYaw, int Index)
{
	FRotator ReturnRot = FirstRotation;
	ReturnRot.Yaw += DeltaYaw * Index;

	return ReturnRot;
}

FVector AGoldenSkull::GetFloorLocationOnHit(const FVector& Location)
{
	UWorld* World = GetWorld();
	FVector ReturnLocation = FVector::ZeroVector;

	if (World)
	{
		TArray<AActor*> IgnoreActors;
		FHitResult Hit;
		FVector End = Location;
		End.Z -= 3000.0f;

		bool IsHit = UKismetSystemLibrary::LineTraceSingle(
			World, 
			Location, 
			End, 
			ETraceTypeQuery::TraceTypeQuery1, 
			false, 
			IgnoreActors, 
			EDrawDebugTrace::ForDuration, 
			Hit, 
			true
		);

		if (IsHit)
		{
			ReturnLocation = Hit.ImpactPoint;
		}
	}

	return ReturnLocation;
}

void AGoldenSkull::ReturnStateToPatrol()
{
	CurrentState = EStateOfEnemy::Patrol;
}
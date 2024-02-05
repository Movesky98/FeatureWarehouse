// Fill out your copyright notice in the Description page of Project Settings.


#include "GoldenSkull.h"
#include "Enums/StateOfEnemy.h"

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

	GetTriggerZone()->OnComponentBeginOverlap.AddDynamic(this, &AGoldenSkull::OnTriggerBeginOverlap);
	GetTriggerZone()->OnComponentEndOverlap.AddDynamic(this, &AGoldenSkull::OnTriggerEndOverlap);
}

void AGoldenSkull::OnTriggerBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, FString("Something in Trigger"));
	CalculateFleeLocation();
}

void AGoldenSkull::OnTriggerEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

/*
* 해보고 싶은 기능이 생김
* 1. 캐릭터의 뒤쪽으로 최대 도망거리만큼 레이를 여러개 쏨 (반원 모양으로)
* 2. 그 중에서 거리가 가장 긴 것을 가져옴.
* 3. 가장 긴 거리의 바닥 (NavMesh때문)의 위치를 가져와서 보내는 방식.
* 
* 4. 반대로, 최소 도망거리보다 짧을 경우 플레이어의 뒤쪽으로 레이를 쏴서 그 방향으로 가도록 만들어보자. 
* 5. 우선 부채꼴 모양으로 레이를 쏘는 방법부터 찾아내는게 중요할 듯.
*/

void AGoldenSkull::CalculateFleeLocation()
{
	// if (CurrentState != EStateOfEnemy::RunAway) return;

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
	}
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
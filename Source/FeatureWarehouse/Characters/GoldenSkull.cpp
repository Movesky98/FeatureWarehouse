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
* �غ��� ���� ����� ����
* 1. ĳ������ �������� �ִ� �����Ÿ���ŭ ���̸� ������ �� (�ݿ� �������)
* 2. �� �߿��� �Ÿ��� ���� �� ���� ������.
* 3. ���� �� �Ÿ��� �ٴ� (NavMesh����)�� ��ġ�� �����ͼ� ������ ���.
* 
* 4. �ݴ��, �ּ� �����Ÿ����� ª�� ��� �÷��̾��� �������� ���̸� ���� �� �������� ������ ������. 
* 5. �켱 ��ä�� ������� ���̸� ��� ������� ã�Ƴ��°� �߿��� ��.
*/

void AGoldenSkull::CalculateFleeLocation()
{
	// if (CurrentState != EStateOfEnemy::RunAway) return;

	UWorld* World = GetWorld();

	if (World)
	{
		// (180�� �� �� Trace�� ����) / ������ ������ ���̸� ��� ���� ���.
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

			// ���� ��ġ, �Ÿ��� ���� ��� ������ �������� ����.
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

		// Step 3. ���� �Ÿ� �� ���� �� �Ÿ��� �ٴ� ��ġ�� �׸�.
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
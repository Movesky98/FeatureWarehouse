// Fill out your copyright notice in the Description page of Project Settings.


#include "RotatorActor.h"

#include "Components/ArrowComponent.h"

#include "DrawDebugHelpers.h"

// Sets default values
ARotatorActor::ARotatorActor()
{
	Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
	RootComponent = Sphere;

	Cone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cone"));
	Cone->SetupAttachment(RootComponent);

	ArrowX = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowX"));
	ArrowY = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowY"));
	ArrowZ = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowZ"));

	ArrowX->SetupAttachment(RootComponent);
	ArrowY->SetupAttachment(RootComponent);
	ArrowZ->SetupAttachment(RootComponent);

	ArrowX->SetArrowColor(FColor::Red);
	ArrowY->SetArrowColor(FColor::Green);
	ArrowZ->SetArrowColor(FColor::Blue);

	Radius = 100.0f;
	AnimateInterval = 0.01f;
	RotSpeed = 5.0f;

	RotationDeltaX = 0.0f;
	RotationDeltaY = 0.0f;
	RotationDeltaZ = 0.0f;

	bIsRotateX = false;
	bIsRotateY = false;
	bIsRotateZ = false;
	bIsReversed = false;
	bIsQuaternion = false;
	LastQuaternionState = false;
}

void ARotatorActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

// Called when the game starts or when spawned
void ARotatorActor::BeginPlay()
{
	Super::BeginPlay();
	
	DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 64, FColor::Black, true, -1, 0, 0.5f);
	// DrawDebugLine(GetWorld(), GetActorLocation() + FVector(-1000.0f, 0.0f, 0.0f), GetActorLocation() + FVector(1000.0f, 0.0f, 0.0f), FColor::Red, true, -1.f, 0, 1);
	// DrawDebugLine(GetWorld(), GetActorLocation() + FVector(0.0f, -1000.0f, 0.0f), GetActorLocation() + FVector(0.0f, 1000.0f, 0.0f), FColor::Green, true, -1.f, 0, 1);
	// DrawDebugLine(GetWorld(), GetActorLocation() + FVector(0.0f, 0.0f, -1000.0f), GetActorLocation() + FVector(0.0f, 0.0f, 1000.0f), FColor::Blue, true, -1.f, 0, 1);

	Cone->SetRelativeLocation(FVector(0.0f, 0.0f, Radius + 50.0f));

	/*ArrowX->bHiddenInGame = false;
	ArrowY->bHiddenInGame = false;
	ArrowZ->bHiddenInGame = false;*/

	ArrowX->ArrowLength = Radius;
	ArrowY->ArrowLength = Radius;
	ArrowZ->ArrowLength = Radius;

	FTimerHandle AnimateTimer;
	GetWorldTimerManager().SetTimer(AnimateTimer, this, &ARotatorActor::Animate, AnimateInterval, true);
}

void ARotatorActor::DrawVector(float Delta)
{
	/*
	* TO DO :
	* 1. Cone의 Direction을 계산
	*	 Cone의 Z를 기준으로 회전 방향을 구현할 생각임.
	*
	* 2. 계산된 Direction을 바탕으로 Arrow X, Y, Z의 방향을 구함.
	* 3. X, Y, Z를 사원수에서 어떻게 구하냐..? 막막하네...
	*/

	/*
	* 오일러각에서의 표현 방법.
	* 1. 짐벌 락 문제를 보여주기 위해 Circle과 Cone의 회전 방향을 표시해주어야 할 듯.
	* 2. X, Y, Z 축에서의 Circle과 Cone의 회전 방향을 가리키는 화살표.
	* 3. X -> Y -> Z 순으로 회전 순서가 정해졌다고 하자.
	* 
	* 쿼터니언에서의 표현 방법
	* 1. 회전 축 -> Q = (w, v)에서의 v (벡터 부분)를 이용해 표현할 것.
	* 2. 계산된 결과 Q, 물체의 실질적인 회전 값을 나타내는 결과값.
	* 3. 회전 각도 -> Q = (w, v)에서의 theta(θ) = 2*arccos(w)
	* 4. 초기 기준 방향 (초기에 물체가 바라보던 방향.)
	* 5. 전체적인 쿼터니언의 값.
	*/

	Direction = Cone->GetUpVector();

	if (bIsQuaternion)
		CalculateQuaternion(Delta);
	else
		CalculateEulerAngle(Delta);
}

void ARotatorActor::CalculateEulerAngle(float Delta)
{
	// 현재 회전 방향
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + Direction * Radius, FColor::Magenta, false, 0.01f, 0, 3);

	FQuat Quat;

	FRotator Rotator = FRotator(0.f, 0.f, RotationDeltaX);

	Quat = FQuat(Rotator);

	// X축
	DrawDebugCircle(GetWorld(), GetActorLocation(), Radius, 64, FColor::Red, false, AnimateInterval, 0, 1, Quat.GetAxisY(), Quat.GetAxisZ());

	Rotator = FRotator(RotationDeltaY, 0.0f, RotationDeltaX);
	Quat = FQuat(Rotator);
	// Y축
	DrawDebugCircle(GetWorld(), GetActorLocation(), Radius, 64, FColor::Green, false, AnimateInterval, 0, 1, Quat.GetAxisZ(), Quat.GetAxisX());

	Rotator = FRotator(RotationDeltaY, RotationDeltaZ, RotationDeltaX);
	Quat = FQuat(Rotator);
	// Z축
	DrawDebugCircle(GetWorld(), GetActorLocation(), Radius, 64, FColor::Blue, false, AnimateInterval, 0, 1, Quat.GetAxisX(), Quat.GetAxisY());
}

void ARotatorActor::CalculateQuaternion(float Delta)
{
	Quaternion = FQuat::MakeFromRotator(Cone->GetRelativeRotation());
	Theta = 2 * FMath::Acos(Quaternion.W);

	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + Quaternion.GetUpVector() * Radius, FColor::Green, false, 0.01f, 0, 2);

	FVector QuatAxis = FVector(Quaternion.X, Quaternion.Y, Quaternion.Z);
	QuatAxis = QuatAxis.GetSafeNormal();

	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + QuatAxis * Radius, FColor::Blue, false, 0.01f, 0, 2);
}

void ARotatorActor::RotateCone(float Delta)
{

	// 현재는 Quaternion으로 계산하는 방식은 넣지 않았음.
	FRotator CurrentRotation = Cone->GetRelativeRotation();

	if (bIsReversed)
	{
		if (bIsRotateX)
		{
			RotationDeltaX = RotationDeltaX <= 0.0f ? 360.0f : RotationDeltaX - Delta * RotSpeed;
			CurrentRotation.Roll = RotationDeltaX;
		}

		if (bIsRotateY)
		{
			RotationDeltaY = RotationDeltaY <= 0.0f ? 360.0f : RotationDeltaY - Delta * RotSpeed;
			CurrentRotation.Pitch = RotationDeltaY;
		}

		if (bIsRotateZ)
		{
			RotationDeltaZ = RotationDeltaZ <= 0.0f ? 360.0f : RotationDeltaZ - Delta * RotSpeed;
			CurrentRotation.Yaw = RotationDeltaZ;
		}

		Cone->SetRelativeRotation(CurrentRotation);

		return;
	}

	if (bIsRotateX)
	{
		RotationDeltaX = RotationDeltaX >= 360.0f ? 0.0f : RotationDeltaX + Delta * RotSpeed;
		CurrentRotation.Roll = RotationDeltaX;
	}

	if (bIsRotateY)
	{
		RotationDeltaY = RotationDeltaY >= 360.0f ? 0.0f : RotationDeltaY + Delta * RotSpeed;
		CurrentRotation.Pitch = RotationDeltaY;
	}

	if (bIsRotateZ)
	{
		RotationDeltaZ = RotationDeltaZ >= 360.0f ? 0.0f : RotationDeltaZ + Delta * RotSpeed;
		CurrentRotation.Yaw = RotationDeltaZ;
	}

	Cone->SetRelativeRotation(CurrentRotation);
}

void ARotatorActor::Animate()
{
	if (LastQuaternionState != bIsQuaternion)
	{
		// 값이 변경되었을 때

		if (OnQuaternionChanged.IsBound())
			OnQuaternionChanged.Broadcast();

		LastQuaternionState = bIsQuaternion;
	}

	DrawVector(AnimateInterval);

	RotateCone(AnimateInterval);
}

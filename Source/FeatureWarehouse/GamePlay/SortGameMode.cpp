// Fill out your copyright notice in the Description page of Project Settings.

#include "GamePlay/SortGameMode.h"
#include "Objects/Sort_NumberBar.h"
#include "Enums/SortType.h"

#define NumberBarsNum 10

ASortGameMode::ASortGameMode()
{

}

void ASortGameMode::PlaySort(ESortType SortType, FVector Location)
{
	if (bIsPlay) return;

	CurrentSortType = SortType;
	bIsPlay = true;

	CurrentIndex = 0;

	SpawnNumberBars(Location);
	// Set NumberBars's rotation to the Player

	// then, start sorting.
	switch (SortType)
	{
	case ESortType::ST_Selection:
		GetWorldTimerManager().SetTimer(StartSortTimer, this, &ASortGameMode::SelectSort, 1.0f, true);
		break;
	case ESortType::ST_Insertion:
		break;
	case ESortType::ST_Bubble:
		break;
	case ESortType::ST_Quick:
		break;
	case ESortType::ST_Heap:
		break;
	case ESortType::ST_Shell:
		break;
	case ESortType::ST_Radix:
		break;
	case ESortType::ST_Counting:
		break;
	default:
		break;
	}
}

void ASortGameMode::BeginPlay()
{
	Super::BeginPlay();

	SpawnNumberBars(FVector::Zero());
}

void ASortGameMode::SpawnNumberBars(FVector Location)
{
	if (!NumberBarClass) return;

	UE_LOG(LogTemp, Warning, TEXT("SortGameMode :: Spawn Number Bars"));

	// 간격은 200
	float Interval = 200.0f;
	FVector SpawnLocation = Location - 5 * Interval * -FVector::RightVector;
	SpawnLocation.Z = 300.0f;

	UWorld* World = GetWorld();

	for (int i = 0; i < NumberBarsNum; i++)
	{
		int32 RandomInteger = FMath::FRandRange(0, 100);

		FRotator Rotation;
		FActorSpawnParameters SpawnParams;

		ASort_NumberBar* NumberBar = World->SpawnActor<ASort_NumberBar>(NumberBarClass, SpawnLocation, Rotation, SpawnParams);
		float NewZ = RandomInteger * 0.02;

		NumberBar->SetNumber(RandomInteger);
		NumberBar->SetNumberBarScale3D_Z(NewZ);
		NumberBar->VisualizeBoundingBox();

		NumberBars.Add(NumberBar);

		SpawnLocation -= Interval * FVector::RightVector;;
	}

	// 넘버 바를 위로 올리는 코드
	FVector GoalLocation = NumberBars[0]->GetActorLocation();
	GoalLocation.Z += 1000.0f;

	FTimerDelegate TimerDel;
	TimerDel.BindUObject(this, &ASortGameMode::MoveNumberBar, GoalLocation, 0);
	GetWorldTimerManager().SetTimer(MoveNumberBarTimer, TimerDel, 0.01f, true);
}

void ASortGameMode::LandNumberBar()
{


}

void ASortGameMode::MoveNumberBar(FVector GoalLocation, int Index)
{
	FVector CurrentLocation = NumberBars[Index]->GetActorLocation();
	FVector InterpolatedVector = FMath::VInterpTo(CurrentLocation, GoalLocation, 0.01f, 5.0f);

	if (FVector::Distance(CurrentLocation, GoalLocation) < 5.0f)
	{
		NumberBars[Index]->SetActorLocation(GoalLocation);
		GetWorldTimerManager().ClearTimer(MoveNumberBarTimer);

		// 이동하는지 확인하는 코드
		/*if (NumberBars.Num() - 1 > Index)
		{
			FTimerDelegate TimerDel;
			GoalLocation = NumberBars[++Index]->GetActorLocation();
			GoalLocation.Z += 1000.0f;
			TimerDel.BindUObject(this, &ASortGameMode::MoveNumberBar, GoalLocation, Index);
			GetWorldTimerManager().SetTimer(MoveNumberBarTimer, TimerDel, 0.01f, true);
		}*/
	}
	else
		NumberBars[Index]->SetActorLocation(InterpolatedVector);
}

void ASortGameMode::SelectSort()
{
	if (CurrentIndex == NumberBars.Num() - 1)
	{

		return;
	}

	for (int i = 0; i < NumberBars.Num() - 1; i++)
	{
		int Index = i;

		for (int j = i + 1; j < NumberBars.Num(); j++)
		{
			if (NumberBars[j] < NumberBars[Index])
				Index = j;
		}

		Swap(NumberBars[i], NumberBars[Index]);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SortGameMode.generated.h"

enum class ESortType :uint8;

UENUM(BlueprintType)
enum class ESortState : uint8
{
	ESS_SelectElement UMETA(DisplayName = "SelectElement"),
	ESS_Compare UMETA(DisplayName = "Compare"),
	ESS_ShoweResult UMETA(DisplayName = "ShowResult"),
	ESS_SwapElement UMETA(DisplayName = "SwapElement")
};

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API ASortGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASortGameMode();

	void PlaySort(ESortType SortType, FVector Location);

protected:
	virtual void BeginPlay() override;

	void SpawnNumberBars(FVector Location);

	UFUNCTION()
	void LandNumberBar();

	UFUNCTION()
	void MoveNumberBar(FVector GoalLocation, int Index);

	FTimerHandle MoveNumberBarTimer;

	FTimerHandle StartSortTimer;

	UFUNCTION()
	void SelectSort();

	int CurrentIndex = 0;

private:
	UPROPERTY(BlueprintReadWrite, Category = "State", meta = (AllowPrivateAccess = "true"))
	TArray<class ASort_NumberBar*> NumberBars;

	UPROPERTY(BlueprintReadWrite, Category = "State", meta = (AllowPrivateAccess = "true"))
	ESortType CurrentSortType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool bIsPlay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool bIsPressedButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ASort_NumberBar> NumberBarClass;
};

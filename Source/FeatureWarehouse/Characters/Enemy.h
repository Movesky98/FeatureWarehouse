// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICharacter.h"
#include "Enemy.generated.h"

enum class EStateOfEnemy :uint8;

UENUM(BlueprintType)
enum class ETypeOfEnemy :uint8
{
	NONE UMETA(DisplayName = "NONE"),
	GoldenSkull UMETA(DisplayName = "GoldenSkull"),
	Archer UMETA(DisplayName = "Archer"),
	Knight UMETA(DisplayName = "Knight")
};

UCLASS()
class FEATUREWAREHOUSE_API AEnemy : public AAICharacter
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	bool IsPlayerApproached();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	ETypeOfEnemy EnemyType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	EStateOfEnemy CurrentState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	bool bHasPlayerApproached;

public:
	FORCEINLINE ETypeOfEnemy GetEnemyType() { return EnemyType; }
};

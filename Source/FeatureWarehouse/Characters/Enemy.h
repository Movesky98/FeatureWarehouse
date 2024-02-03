// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICharacter.h"
#include "Enemy.generated.h"

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
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	ETypeOfEnemy EnemyType;

public:
	FORCEINLINE ETypeOfEnemy GetEnemyType() { return EnemyType; }
};

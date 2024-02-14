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
	AEnemy();

	UFUNCTION(BlueprintCallable)
	bool IsPlayerApproached();

	virtual EStateOfEnemy IdentifiedPlayer();

	virtual void EngagingInCombat(AActor* AdversaryActor);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	ETypeOfEnemy EnemyType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	EStateOfEnemy CurrentState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	bool bIsPlayerApproached;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Range", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AttackRangeComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Range", meta = (AllowPrivateAccess = "true"))
	float AttackRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|State", meta = (AllowPrivateAccess = "true"))
	bool bCanAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|State", meta = (AllowPrivateAccess = "true"))
	bool bIsPlayerInAttackRange;

	UFUNCTION(BlueprintCallable)
	void SetMovemenetSpeed(float Speed);

public:
	FORCEINLINE ETypeOfEnemy GetEnemyType() { return EnemyType; }
};

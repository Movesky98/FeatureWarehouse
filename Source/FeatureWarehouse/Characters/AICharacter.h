// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AICharacter.generated.h"

UCLASS()
class FEATUREWAREHOUSE_API AAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	float Velocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Trigger, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* TriggerZone;

public:
	FORCEINLINE class USphereComponent* GetTriggerZone() { return TriggerZone; }
};

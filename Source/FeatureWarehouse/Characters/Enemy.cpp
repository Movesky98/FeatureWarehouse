// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Enums/StateOfEnemy.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"

AEnemy::AEnemy()
{
	AttackRangeComponent = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeComponent"));
	AttackRangeComponent->SetupAttachment(RootComponent);
	AttackRangeComponent->SetCollisionProfileName(FName("PlayerTrigger"));
	AttackRangeComponent->SetGenerateOverlapEvents(true);
	AttackRangeComponent->InitSphereRadius(AttackRange);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AttackRangeComponent->SetSphereRadius(AttackRange);
}

bool AEnemy::IsPlayerApproached()
{
	return bIsPlayerApproached;
}

void AEnemy::SetMovemenetSpeed(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

EStateOfEnemy AEnemy::IdentifiedPlayer()
{
	return EStateOfEnemy::Patrol;
}

void AEnemy::EngagingInCombat(AActor* AdversaryActor)
{

}

void AEnemy::Attack()
{

}

bool AEnemy::CanAttack()
{
	bool CanAttack = !UKismetMathLibrary::BooleanXOR(bCanCombo, bIsAttacking);

	return CanAttack;
}
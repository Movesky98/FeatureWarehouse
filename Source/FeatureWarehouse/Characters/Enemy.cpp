// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Enums/StateOfEnemy.h"

bool AEnemy::IsPlayerApproached()
{
	return bIsPlayerApproached;
}

EStateOfEnemy AEnemy::IdentifiedPlayer()
{
	return EStateOfEnemy::Patrol;
}
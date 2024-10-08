// Fill out your copyright notice in the Description page of Project Settings.


#include "CriticalDamageType.h"

void UCriticalDamageType::SetReactionMontage(UAnimMontage* Montage)
{
	ReactionMontage = Montage;
}

UAnimMontage* UCriticalDamageType::GetReactionMontage() const
{
	return ReactionMontage;
}

void UCriticalDamageType::SetDeathMontage(UAnimMontage* Montage)
{
	DeathMontage = Montage;
}

UAnimMontage* UCriticalDamageType::GetDeathMontage() const
{
	return DeathMontage;
}

void UCriticalDamageType::SetPlayLocation(FVector Location)
{
	PlayLocation = Location;
}

FVector UCriticalDamageType::GetPlayLocation() const
{
	return PlayLocation;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

#include "Components/ArrowComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "Projectile.h"
#include "Enums/UseTypeOfWeapon.h"
#include "Enums/FireMode.h"


AGun::AGun()
{
	MuzzleArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("MuzzleArrow"));
	MuzzleArrow->SetupAttachment(GetSkeletalMesh());

	HiveShotArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("HiveShotArrow"));
	HiveShotArrow->SetupAttachment(GetSkeletalMesh());
}

void AGun::Attack()
{
	switch (FireMode)
	{
	case EFireMode::SemiAuto:
		break;

	case EFireMode::FullAuto:
		break;

	case EFireMode::Burst:
		break;

	default:
		break;
	}
}

void AGun::StartFire()
{

}

void AGun::StopFire()
{

}

void AGun::Fire()
{

}
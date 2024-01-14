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

	BurstIndex = 0;
}

void AGun::Attack()
{
	switch (FireMode)
	{
	case EFireMode::SemiAuto:
		Fire();
		break;

	case EFireMode::FullAuto:
		StartFire();
		break;

	case EFireMode::Burst:
		StartFire();
		break;
	default:
		break;
	}
}

void AGun::StopAttack()
{
	if (FireMode == EFireMode::SemiAuto || FireMode == EFireMode::Burst) return;

	StopFire();
}

void AGun::StartFire()
{
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AGun::Fire, FireRate, true);
}

void AGun::StopFire()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);

	if (FireMode == EFireMode::Burst) 
		BurstIndex = 0;
}

void AGun::Fire()
{
	// not implemented whether an attack is possible.

	if (Info.CurrentRounds <= 0)
	{
		// Reload
		if(Info.TotalRounds >= Info.MagazineSize)
			Info.CurrentRounds = Info.MagazineSize;
		else
		{
			Info.CurrentRounds = Info.TotalRounds;
			Info.TotalRounds = 0;
		}
		return;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		FVector Location = MuzzleArrow->GetComponentLocation();
		FRotator Rotation = MuzzleArrow->GetComponentRotation();

		AProjectile* Projectile = World->SpawnActor<AProjectile>(Info.ProjectileClass, Location, Rotation);
		
		if(Projectile)
		{
			Info.CurrentRounds--;
			
			FVector Direction = MuzzleArrow->GetForwardVector();
			Projectile->FireDirection(Direction);

			if (FireMode == EFireMode::Burst)
				BurstIndex < 3 ? BurstIndex++ : StopFire();
		}
	}
}
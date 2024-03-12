// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "Projectile.h"
#include "GamePlay/FW_PlayerController.h"
#include "Enums/StateOfViews.h"
#include "Enums/UseTypeOfWeapon.h"
#include "Enums/FireMode.h"

#include "AnimInstance/PlayerAnimInstance.h"
#include "GameFramework/Character.h"

AGun::AGun()
{
	MuzzleArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("MuzzleArrow"));
	MuzzleArrow->SetupAttachment(GetSkeletalMesh());

	HiveShotArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("HiveShotArrow"));
	HiveShotArrow->SetupAttachment(GetSkeletalMesh());

	BurstIndex = 0;
}

void AGun::Equip()
{
	AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, EquipName);
}

void AGun::Unequip()
{
	AttachToComponent(GetWeaponOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, GetAttachSocketName());
}

void AGun::Attack(EStateOfViews CurView, FVector HitLocation)
{
	switch (FireMode)
	{
	case EFireMode::SemiAuto:
		Fire(CurView, HitLocation);
		break;
	case EFireMode::FullAuto:
	case EFireMode::Burst:
		StartFire(CurView, HitLocation);
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

void AGun::StartFire(EStateOfViews CurView, FVector HitLocation)
{
	FTimerDelegate FireTimerDel;
	FireTimerDel.BindUFunction(this, FName("Fire"), CurView, HitLocation);

	GetWorldTimerManager().SetTimer(FireTimerHandle, FireTimerDel, FireRate, true);
}

void AGun::StopFire()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);

	if (FireMode == EFireMode::Burst) 
		BurstIndex = 0;
}

void AGun::Fire(EStateOfViews CurView, FVector HitLocation)
{
	// not implement whether an attack is possible.

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
	
	switch (CurView)
	{
	case EStateOfViews::TPP:
		TPPFire();
		break;
	case EStateOfViews::FPP:
		FPPFire();
		break;
	case EStateOfViews::TDP:
		TDPFire(HitLocation);
		break;
	}
}

// Gun firing when the player's view is first person view.
void AGun::FPPFire()
{
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (!CameraManager) return;

	UWorld* World = GetWorld();
	if (World)
	{
		FVector CameraStart = CameraManager->GetCameraLocation();
		FVector CameraEnd = CameraStart + UKismetMathLibrary::GetForwardVector(CameraManager->GetCameraRotation()) * 50000.0f;
		TArray<AActor*> IgnoreActors;
		FHitResult Hit;

		bool IsHit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			CameraStart,
			CameraEnd,
			ETraceTypeQuery::TraceTypeQuery1,
			true,
			IgnoreActors,
			EDrawDebugTrace::ForDuration,
			Hit,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			1.0f
		);

		FVector FireLocation = MuzzleArrow->GetComponentLocation();
		FRotator FireRotation;
		
		IsHit ? FireRotation = UKismetMathLibrary::FindLookAtRotation(FireLocation, Hit.ImpactPoint) 
			: FireRotation = UKismetMathLibrary::FindLookAtRotation(FireLocation, CameraEnd);

		SpawnParticles();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = GetWeaponOwner();

		AProjectile* Projectile = World->SpawnActor<AProjectile>(Info.ProjectileClass, FireLocation, FireRotation, SpawnParams);
		if (Projectile)
		{
			Info.CurrentRounds--;

			FVector Direction = UKismetMathLibrary::GetForwardVector(FireRotation);
			Projectile->FireDirection(Direction);

			if (FireMode == EFireMode::Burst)
				BurstIndex < 3 ? BurstIndex++ : StopFire();
		}
	}
}

// Gun firing when the player's view is third person view.
void AGun::TPPFire()
{
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (!CameraManager) return;

	UWorld* World = GetWorld();
	if (World)
	{
		// Camera trace.
		FVector CameraStart = CameraManager->GetCameraLocation();
		FVector CameraEnd = CameraStart + UKismetMathLibrary::GetForwardVector(CameraManager->GetCameraRotation()) * 50000.0f;
		TArray<AActor*> IgnoreActors;
		FHitResult Hit;

		bool IsHit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			CameraStart,
			CameraEnd,
			ETraceTypeQuery::TraceTypeQuery1,
			true,
			IgnoreActors,
			EDrawDebugTrace::ForDuration,
			Hit,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			0.0f
		);

		FVector FireLocation;
		FRotator FireRotation;

		if (IsHit)
		{
			FireLocation = MuzzleArrow->GetComponentLocation();
			FireRotation = UKismetMathLibrary::FindLookAtRotation(FireLocation, Hit.ImpactPoint);
		}
		else
		{
			FireLocation = MuzzleArrow->GetComponentLocation();
			FireRotation = UKismetMathLibrary::FindLookAtRotation(FireLocation, CameraEnd);
		}

		SpawnParticles();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = GetWeaponOwner();

		AProjectile* Projectile = World->SpawnActor<AProjectile>(Info.ProjectileClass, FireLocation, FireRotation, SpawnParams);
		if (Projectile)
		{
			Info.CurrentRounds--;

			FVector Direction = UKismetMathLibrary::GetForwardVector(FireRotation);
			Projectile->FireDirection(Direction);

			if (FireMode == EFireMode::Burst)
				BurstIndex < 3 ? BurstIndex++ : StopFire();
		}
	}
}

// Gun firing when the player's view is top down view.
void AGun::TDPFire(FVector HitLocation)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FVector FireLocation = MuzzleArrow->GetComponentLocation();;
		FVector FireVector = HitLocation - FireLocation;
		FRotator FireRotation = UKismetMathLibrary::MakeRotFromX(FireVector);

		SpawnParticles();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = GetWeaponOwner();
		AProjectile* Projectile = World->SpawnActor<AProjectile>(Info.ProjectileClass, FireLocation, FireRotation, SpawnParams);
		if (Projectile)
		{
			Info.CurrentRounds--;

			FVector Direction = UKismetMathLibrary::GetForwardVector(FireRotation);
			Projectile->FireDirection(Direction);

			if (FireMode == EFireMode::Burst)
				BurstIndex < 3 ? BurstIndex++ : StopFire();
		}
	}
}

void AGun::SpawnParticles()
{
	UWorld* World = GetWorld();
	if (World)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Info.NS_MuzzleFlash, MuzzleArrow->GetComponentLocation(), MuzzleArrow->GetComponentRotation());
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Info.NS_HiveShot, HiveShotArrow->GetComponentLocation(), HiveShotArrow->GetComponentRotation());
	}
}

void AGun::DrawMuzzleLineTrace()
{
	FVector Start;
	FVector End;

	Start = MuzzleArrow->GetComponentLocation();
	End = MuzzleArrow->GetComponentLocation() + UKismetMathLibrary::GetForwardVector(MuzzleArrow->GetComponentRotation()) * 50000.0f;

	TArray<AActor*> IgnoreActor;
	FHitResult Hit;

	bool IsHit = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		Start,
		End,
		ETraceTypeQuery::TraceTypeQuery1,
		true,
		IgnoreActor,
		EDrawDebugTrace::ForDuration,
		Hit,
		true
	);
}
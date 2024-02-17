// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GamePlayStatics.h"
#include "Particles/ParticleSystem.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_Blood(TEXT("/Game/ImportAssets/Realistic_Starter_VFX_Pack_Vol2/Particles/Blood/P_Blood_Splat_Cone"));
	if (P_Blood.Succeeded())
	{
		BloodParticle = P_Blood.Object;
	}
}


// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

}

bool UStatComponent::GetDamaged(float Damage)
{
	if (!bIsDamagable) return false;

	bIsDamagable = false;

	CurrentHP -= Damage;

	if (CurrentHP <= 0.0f)
	{
		USkeletalMeshComponent* OwnerSkeletalMesh = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

		if (OwnerSkeletalMesh)
		{
			OwnerSkeletalMesh->SetSimulatePhysics(true);

			return true;
		}
	}
	else
	{
		FTimerHandle DamagableTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(DamagableTimerHandle, this, &UStatComponent::SetDamagable, 0.1f);
	}

	return true;
}

void UStatComponent::ShowBloodEffect(FVector Location, FRotator Rotation)
{
	UWorld* World = GetWorld();
	if (World)
	{
		UGameplayStatics::SpawnEmitterAtLocation(World, BloodParticle, Location, Rotation, true);
	}
}
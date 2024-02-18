// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"
#include "Animation/AnimMontage.h"

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

	if (GetDamagedMontage)
	{
		PlayMontage(GetDamagedMontage);
	}

	if (CurrentHP <= 0.0f)
	{
		USkeletalMeshComponent* OwnerSkeletalMesh = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

		if (OwnerSkeletalMesh)
		{
			if (DeathMontage)
			{
				PlayMontage(DeathMontage);
			}
			else
			{
				OwnerSkeletalMesh->SetSimulatePhysics(true);
			}

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

void UStatComponent::PlayMontage(UAnimMontage* PlayMontage)
{
	USkeletalMeshComponent* OwnerSkeletalMesh = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	
	if (!OwnerSkeletalMesh) return;
	
	UAnimInstance* OwnerAnim = OwnerSkeletalMesh->GetAnimInstance();
	if (OwnerAnim)
	{
		// 재생되고 있는 몽타주 중지.
		OwnerAnim->Montage_Stop(0.0f, nullptr);

		// 몽타주 재생 (피격, 죽음)
		OwnerAnim->Montage_Play(PlayMontage);
	}
	
}
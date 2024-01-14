// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UHealthComponent::GetDamaged(float Damage)
{
	if (!bIsDamagable) return;

	bIsDamagable = false;

	ChangeDamagedMaterial();

	CurrentHP -= Damage;

	if (CurrentHP <= 0.0f)
	{
		UActorComponent* ActorComponent = GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass());
		
		if (ActorComponent)
		{
			USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(ActorComponent);

			SkeletalMesh->SetSimulatePhysics(true);
		}
	}
	else
	{
		FTimerHandle DamagableTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(DamagableTimerHandle, this, &UHealthComponent::SetDamagable, 0.1f);
	}
}

// USkeletalMeshComponent, UStaticMeshComponent 둘 중 하나만 있는 경우만 가능함.
void UHealthComponent::ChangeDamagedMaterial()
{
	FTimerDelegate RestoreTimerDel;
	FTimerHandle RestoreTimerHandle;
	bool IsStaticMesh = false;

	UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

	if (StaticMeshComponent)
	{
		IsStaticMesh = true;
		
		StaticMeshComponent->SetVectorParameterValueOnMaterials(FName("Tint"), FVector(1.0f, 0.0f, 0.0f));

		RestoreTimerDel.BindUFunction(this, FName("RestoreOriginalMaterial"), IsStaticMesh);
		GetWorld()->GetTimerManager().SetTimer(RestoreTimerHandle, RestoreTimerDel, 0.1f, false);

		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

	if (SkeletalMeshComponent)
	{
		TArray<UMaterialInterface*> BaseMaterials = SkeletalMeshComponent->GetMaterials();
		
		SkeletalMeshComponent->SetVectorParameterValueOnMaterials(FName("Tint"), FVector(1.0f, 0.0f, 0.0f));

		RestoreTimerDel.BindUFunction(this, FName("RestoreOriginalMaterial"), IsStaticMesh);
		GetWorld()->GetTimerManager().SetTimer(RestoreTimerHandle, RestoreTimerDel, 0.1f, false);

		return;
	}
}

void UHealthComponent::RestoreOriginalMaterial(bool bIsStaticMeshComponent)
{
	if (bIsStaticMeshComponent)
	{
		UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		
		StaticMeshComponent->SetVectorParameterValueOnMaterials(FName("Tint"), FVector(1.0f, 1.0f, 1.0f));
	}
	else
	{
		USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

		SkeletalMeshComponent->SetVectorParameterValueOnMaterials(FName("Tint"), FVector(1.0f, 1.0f, 1.0f));
	}
}
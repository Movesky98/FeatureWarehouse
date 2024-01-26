// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"
#include "GamePlay/FW_GameInstance.h"
#include "Widgets/PlayerMenu.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GamePlayStatics.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerSkeletalMesh = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (OwnerSkeletalMesh)
		bIsSkeletalMeshComponent = true;

	if (bIsPlayer)
	{
		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());

		if (GameInstance)
		{
			UFW_GameInstance* FWGameInstance = Cast<UFW_GameInstance>(GameInstance);
			PlayerMenu = FWGameInstance->PlayerMenu;
			PlayerMenu->UpdatePlayerHealth(CurrentHP, MaxHP);
		}
	}
}

void UStatComponent::GetDamaged(float Damage)
{
	if (!bIsDamagable) return;

	bIsDamagable = false;

	ChangeDamagedMaterial();

	CurrentHP -= Damage;

	if (bIsPlayer)
	{
		PlayerMenu->UpdatePlayerHealth(CurrentHP, MaxHP);
	}

	if (CurrentHP <= 0.0f)
	{
		if (OwnerSkeletalMesh)
		{
			OwnerSkeletalMesh->SetSimulatePhysics(true);
		}
	}
	else
	{
		FTimerHandle DamagableTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(DamagableTimerHandle, this, &UStatComponent::SetDamagable, 0.1f);
	}
}

// USkeletalMeshComponent, UStaticMeshComponent 둘 중 하나만 있는 경우만 가능함.
void UStatComponent::ChangeDamagedMaterial()
{
	FTimerHandle RestoreTimerHandle;

	if (!bIsSkeletalMeshComponent)
	{
		UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

		StaticMeshComponent->SetVectorParameterValueOnMaterials(FName("Tint"), FVector(1.0f, 0.0f, 0.0f));

		GetWorld()->GetTimerManager().SetTimer(RestoreTimerHandle, this, &UStatComponent::RestoreOriginalMaterial, 0.1f, false);

		return;
	}

	if (OwnerSkeletalMesh)
	{
		TArray<UMaterialInterface*> BaseMaterials = OwnerSkeletalMesh->GetMaterials();

		OwnerSkeletalMesh->SetVectorParameterValueOnMaterials(FName("Tint"), FVector(1.0f, 0.0f, 0.0f));

		GetWorld()->GetTimerManager().SetTimer(RestoreTimerHandle, this, &UStatComponent::RestoreOriginalMaterial, 0.1f, false);

		return;
	}
}

void UStatComponent::RestoreOriginalMaterial()
{
	if (bIsSkeletalMeshComponent)
	{
		OwnerSkeletalMesh->SetVectorParameterValueOnMaterials(FName("Tint"), FVector(1.0f, 1.0f, 1.0f));
	}
	else
	{
		UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

		StaticMeshComponent->SetVectorParameterValueOnMaterials(FName("Tint"), FVector(1.0f, 1.0f, 1.0f));
	}
}
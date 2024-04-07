// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverRegion.h"

#include "Characters/WeaponWielder.h"

#include "Components/BoxComponent.h"

// Sets default values
AGameOverRegion::AGameOverRegion()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	BoxComponent->SetCollisionProfileName(FName("PlayerTrigger"));
}

void AGameOverRegion::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BoxComponent->SetGenerateOverlapEvents(true);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGameOverRegion::OnRegionBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AGameOverRegion::OnRegionEndOverlap);
}

// Called when the game starts or when spawned
void AGameOverRegion::BeginPlay()
{
	Super::BeginPlay();
	
	if (!Wielders.IsEmpty())
	{
		for (auto Wielder : Wielders)
		{
			
		}
	}
}

void AGameOverRegion::OnRegionBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AGameOverRegion::OnRegionEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

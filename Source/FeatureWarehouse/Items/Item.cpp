// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "../Enums/TypeOfItem.h"

#include "Components/SphereComponent.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	RootComponent = SkeletalMesh;
	SkeletalMesh->SetCollisionProfileName(FName("NoCollision"));

	TriggerZone = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerZone"));
	TriggerZone->SetupAttachment(SkeletalMesh);
	TriggerZone->SetCollisionProfileName(FName("Item"));

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

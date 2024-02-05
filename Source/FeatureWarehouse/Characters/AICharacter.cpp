// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AICharacter.h"
#include "GamePlay/EnemyAIController.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
AAICharacter::AAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = GetCapsuleComponent();

	TriggerZone = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerZone"));
	TriggerZone->SetupAttachment(RootComponent);
	TriggerZone->SetCollisionProfileName(FName("PlayerTrigger"));
	TriggerZone->SetGenerateOverlapEvents(true);

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
	Super::BeginPlay();

}

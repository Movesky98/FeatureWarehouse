// Fill out your copyright notice in the Description page of Project Settings.


#include "Sort_PlayButton.h"

#include "GamePlay/SortGameMode.h"
#include "Enums/SortType.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ASort_PlayButton::ASort_PlayButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMeshComponent;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Cube(TEXT("/Game/ImportAssets/A_Surface_Footstep/Environment_Assets/1M_Cube"));
	if (SM_Cube.Succeeded())
	{
		StaticMeshComponent->SetStaticMesh(SM_Cube.Object);
	}

	StaticMeshComponent->SetRelativeScale3D(FVector(0.05f, 0.2f, 0.1f));
}

void ASort_PlayButton::Interact(AActor* InteractActor)
{
	ASortGameMode* SortGameMode = Cast<ASortGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (SortGameMode != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("You should change the game mode of the world to SortGameMode."));
		return;
	}

}

// Called when the game starts or when spawned
void ASort_PlayButton::BeginPlay()
{
	Super::BeginPlay();
	
}


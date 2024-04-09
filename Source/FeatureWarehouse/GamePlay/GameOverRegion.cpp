// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverRegion.h"
#include "FW_GameMode.h"

#include "Characters/WeaponWielder.h"

#include "Components/BoxComponent.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AGameOverRegion::AGameOverRegion()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	BoxComponent->SetCollisionProfileName(FName("PlayerTrigger"));

	WielderCount = 0;
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
	
	if (GameOverType != EGameOverType::EGO_KillAllEnemies) return;

	if (!Wielders.IsEmpty())
	{
		for (auto Wielder : Wielders)
		{
			WielderCount++;
			Wielder->OnKilled.AddUObject(this, &AGameOverRegion::DecreaseWielderCount);
		}
	}
}

void AGameOverRegion::DecreaseWielderCount(AActor* Actor)
{
	AWeaponWielder* Wielder = Cast<AWeaponWielder>(Actor);
	if (!Wielder) return;

	if (Wielders.Find(Wielder) != INDEX_NONE)
	{
		WielderCount--;

		if (WielderCount == 0)
		{
			NotifyGameOver();
		}
	}
}

void AGameOverRegion::OnRegionBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (Player != OtherActor) return;

	if (GameOverType == EGameOverType::EGO_ReachRegion)
	{
		NotifyGameOver();
	}
}

void AGameOverRegion::OnRegionEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AGameOverRegion::NotifyGameOver()
{
	AFW_GameMode* GameMode = Cast<AFW_GameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->HandlerGameOver();
	}
}

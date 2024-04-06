// Fill out your copyright notice in the Description page of Project Settings.


#include "FW_GameMode.h"
#include "FW_GameInstance.h"

#include "Characters/WeaponWielder.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFW_GameMode::AFW_GameMode()
{
	EnemyKills = 0;
	
}

void AFW_GameMode::BeginPlay()
{
	Super::BeginPlay();

}

void AFW_GameMode::BindCharacterDeathEvent(AWeaponWielder* Character)
{
	Character->OnKilled.BindUObject(this, &AFW_GameMode::CheckCharacterDead);

	UE_LOG(LogTemp, Warning, TEXT("GameMode || Bind the character's death event."))
}

void AFW_GameMode::CheckCharacterDead(bool bIsPlayer)
{
	if (bIsPlayer)
	{
		// Load GameEndUI
		UFW_GameInstance* GameInstance = Cast<UFW_GameInstance>(GetGameInstance());

		if (GameInstance)
		{
			GameInstance->LoadGameEndMenu();
		}
	}
	else
	{
		// Increase player's enemy kill.
		EnemyKills++;
	}
}

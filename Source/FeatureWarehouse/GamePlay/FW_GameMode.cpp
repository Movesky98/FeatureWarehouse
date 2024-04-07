// Fill out your copyright notice in the Description page of Project Settings.


#include "FW_GameMode.h"
#include "FW_GameInstance.h"

#include "Widgets/GameEndMenu.h"

#include "Characters/WeaponWielder.h"
#include "Characters/PlayerCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AFW_GameMode::AFW_GameMode()
{
	EnemyKills = 0;
	EnemyNum = 0;
}

void AFW_GameMode::BeginPlay()
{
	Super::BeginPlay();

}

void AFW_GameMode::BindCharacterDeathEvent(AWeaponWielder* Character)
{
	Character->OnKilled.BindUObject(this, &AFW_GameMode::CheckCharacterDead);

	if (Character->IsA<APlayerCharacter>()) return;

	EnemyNum++;
}

void AFW_GameMode::CheckCharacterDead(bool bIsPlayer)
{
	if (bIsPlayer)
	{
		// Load GameEndUI
		UFW_GameInstance* GameInstance = Cast<UFW_GameInstance>(GetGameInstance());

		if (GameInstance)
		{
			GameDuration = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
			GameInstance->GameOver(EnemyKills, GameDuration, TotalScore);
		}
	}
	else
	{
		// Increase player's enemy kill.
		EnemyKills++;
	}
}

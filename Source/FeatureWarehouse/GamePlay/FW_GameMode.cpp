// Fill out your copyright notice in the Description page of Project Settings.


#include "FW_GameMode.h"
#include "FW_GameInstance.h"

#include "Widgets/GameEndMenu.h"

#include "Characters/WielderBase.h"
#include "Characters/PlayerCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AFW_GameMode::AFW_GameMode()
{
	EnemyKills = 0;
	EnemyCount = 0;
}

void AFW_GameMode::BeginPlay()
{
	Super::BeginPlay();

}

void AFW_GameMode::BindCharacterDeathEvent(AWielderBase* Character)
{
	Character->OnKilled.AddUObject(this, &AFW_GameMode::CheckCharacterDead);

	if (Character->IsA<APlayerCharacter>()) return;

	Wielders.Add(Character);

	EnemyCount++;
}

void AFW_GameMode::HandlerGameOver()
{
	if (EnemyKills > EnemyCount) return;

	UFW_GameInstance* GameInstance = Cast<UFW_GameInstance>(GetGameInstance());
	if (!GameInstance) return;

	GameDuration = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
	GameInstance->GameClear(EnemyKills, GameDuration, TotalScore);
}

void AFW_GameMode::CheckCharacterDead(AActor* Actor)
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player) return;

	if (Actor == Player)
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
		AWielderBase* Wielder = Cast<AWielderBase>(Actor);
		if (Wielder->GetKilledByWielder() == Player)
		{
			EnemyKills++;
		}
	}
}

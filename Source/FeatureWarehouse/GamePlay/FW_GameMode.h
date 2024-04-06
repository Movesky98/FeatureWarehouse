// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FW_GameMode.generated.h"

class AWeaponWielder;

UCLASS()
class FEATUREWAREHOUSE_API AFW_GameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFW_GameMode();

	void BindCharacterDeathEvent(class AWeaponWielder* Character);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void CheckCharacterDead(bool bIsPlayer);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool bIsPlayerDead;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	int EnemyKills;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	float TimeTakenToClear;
};

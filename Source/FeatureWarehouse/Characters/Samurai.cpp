// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Samurai.h"

ASamurai::ASamurai()
{
	static ConstructorHelpers::FObjectFinder<UBlueprint> BP_Katana(TEXT("/Game/Project/Blueprints/Items/Melee/BP_Katana"));
	if (BP_Katana.Succeeded())
	{
		WeaponBlueprint = (UClass*)BP_Katana.Object->GeneratedClass;
	}
}
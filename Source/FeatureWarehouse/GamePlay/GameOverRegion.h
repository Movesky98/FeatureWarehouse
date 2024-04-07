// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameOverRegion.generated.h"

UENUM(BlueprintType)
enum class EGameOverType :uint8
{
	EGO_NONE = 0 UMETA(DisplayName = "NONE"),
	EGO_KillAllEnemies UMETA(DisplayName = "Kill All Enemies"),
	EGO_ReachRegion UMETA(DisplayName = "Reach")
};

UCLASS()
class FEATUREWAREHOUSE_API AGameOverRegion : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameOverRegion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnRegionBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRegionEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	class UBoxComponent* BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	TArray<class AWeaponWielder*> Wielders;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State")
	EGameOverType GameOverType;

};
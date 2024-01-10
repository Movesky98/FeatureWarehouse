// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/InteractInterface.h"
#include "Item.generated.h"

enum class ETypeOfItem :uint8;

UCLASS()
class FEATUREWAREHOUSE_API AItem : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	FORCEINLINE USkeletalMeshComponent* GetSkeletalMesh() { return SkeletalMesh; }

	FORCEINLINE class USphereComponent* GetTriggerZone() { return TriggerZone; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Trigger, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* TriggerZone;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	ETypeOfItem ItemType;
};

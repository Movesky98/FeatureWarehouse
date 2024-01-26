// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dummy.generated.h"

class USkeletalMeshComponent;

UCLASS()
class FEATUREWAREHOUSE_API ADummy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADummy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMesh;
};

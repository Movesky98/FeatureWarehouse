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

	UTexture2D* GetItemTexture();

	FORCEINLINE class USphereComponent* GetTriggerZone() { return TriggerZone; }
	FORCEINLINE class USphereComponent* GetItemCollision() { return ItemCollision; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnTriggerBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnTriggerEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void ActiveOverlay();

	virtual void DeactiveOverlay();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overlay")
	TObjectPtr<UMaterialInterface> OutlineMaterial;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Description")
	class UItemDescriptionComponent* ItemDescriptionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* ItemCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* TriggerZone;
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	ETypeOfItem ItemType;
};

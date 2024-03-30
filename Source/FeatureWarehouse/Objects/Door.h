// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"

#include "Door.generated.h"

class UBoxComponent;
class UArrowComponent;

UCLASS()
class FEATUREWAREHOUSE_API ADoor : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

	void Interact(AActor* InteractActor) override;

protected:
	virtual void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTriggerBeginOverlap(class UPrimitiveComponent* selfComp, class AActor* otherActor, UPrimitiveComponent* otherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEndOverlap(class UPrimitiveComponent* selfComp, class AActor* otherActor, UPrimitiveComponent* otherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void SetDoorRotation();

	void OpenDoor();

	void CloseDoor();

	void LoadLevel(FName LoadToLevel);

	void UnloadLevel(FName UnloadToLevel);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FName NextLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FName CurrentLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* DoorFrame;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* Door;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	UBoxComponent* TriggerBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	UArrowComponent* FrontDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsNearTheCharacter;	// ĳ���Ͱ� ��ó�� �ִ��� (���� �� �� �ִ� ��������)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bOpenedFromBehind;		// ĳ���Ͱ� �� �ڿ��� ��������

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDoorMoving;			// ���� �����̰� �ִ���

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsOpen;				// ���� �����ִ���

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	float OpenLimitAngle;		// �� ���� �ִ� ����

	FTimerHandle DoorTimerHandle;
};

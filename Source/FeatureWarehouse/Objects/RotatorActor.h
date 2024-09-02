// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RotatorActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuaternionChanged);

class UArrowComponent;

UCLASS()
class FEATUREWAREHOUSE_API ARotatorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARotatorActor();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnQuaternionChanged OnQuaternionChanged;

protected:
	virtual void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void DrawVector(float Delta);

	void CalculateEulerAngle(float Delta);

	void CalculateQuaternion(float Delta);

	void RotateCone(float Delta);

	UFUNCTION()
	void Animate();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Sphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Cone;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arrow", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* ArrowX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arrow", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* ArrowY;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arrow", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* ArrowZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info", meta = (AllowPrivateAccess = "true"))
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction", meta = (AllowPrivateAccess = "true"))
	float RotSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Direction", meta = (AllowPrivateAccess = "true"))
	FVector Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction", meta = (AllowPrivateAccess = "true"))
	bool bIsRotateX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction", meta = (AllowPrivateAccess = "true"))
	float RotationDeltaX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction", meta = (AllowPrivateAccess = "true"))
	bool bIsRotateY;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction", meta = (AllowPrivateAccess = "true"))
	float RotationDeltaY;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction", meta = (AllowPrivateAccess = "true"))
	bool bIsRotateZ;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction", meta = (AllowPrivateAccess = "true"))
	float RotationDeltaZ;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction", meta = (AllowPrivateAccess = "true"))
	bool bIsReversed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction", meta = (AllowPrivateAccess = "true"))
	bool bIsQuaternion;

	UPROPERTY()
	bool LastQuaternionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Direction", meta = (AllowPrivateAccess = "true"))
	FQuat Quaternion;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Direction", meta = (AllowPrivateAccess = "true"))
	float Theta;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction", meta = (AllowPrivateAccess = "true"))
	float AnimateInterval;
};

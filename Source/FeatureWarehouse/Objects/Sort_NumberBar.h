// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sort_NumberBar.generated.h"

UCLASS()
class FEATUREWAREHOUSE_API ASort_NumberBar : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASort_NumberBar();

	UFUNCTION(BlueprintSetter)
	void SetNumber(int Num);

	UFUNCTION(BlueprintGetter)
	int GetNumber()
	{
		return Number;
	}

	void VisualizeBoundingBox();

	UFUNCTION(BlueprintSetter)
	void SetNumberBarScale3D_Z(float Z);

	UFUNCTION(BlueprintGetter)
	float GetNumberBarScale3D_Z();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void ChangeColor(FColor Color);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	int Number;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintREadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* TextRenderComponent;
};

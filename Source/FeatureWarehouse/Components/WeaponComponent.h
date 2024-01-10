// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

class AWeapon;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FEATUREWAREHOUSE_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

	UFUNCTION(BlueprintCallable)
	void SaveWeaponInfo(AWeapon* NewWeapon);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
private:
	// Functions
	void SetWeaponInfo(AWeapon* NewWeapon, bool IsFirst);

	UFUNCTION(BlueprintCallable)
	void NotifyToAnimInstance();

	// Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	AWeapon* FirstWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	AWeapon* SecondWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	int EquipNum;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	class UPlayerAnimInstance* PlayerAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	class APlayerCharacter* PlayerCharacter;

#pragma region GetterSetter
public:
	FORCEINLINE AWeapon* GetFirstWeapon() const { return FirstWeapon; }

	FORCEINLINE void SetFirstWeapon(AWeapon* NewWeapon) { FirstWeapon = NewWeapon; }

	FORCEINLINE AWeapon* GetSecondWeapon() const { return SecondWeapon; }

	FORCEINLINE void SetSecondWeapon(AWeapon* NewWeapon) { SecondWeapon = NewWeapon; }

	FORCEINLINE int GetEquipNum() { return EquipNum; }

#pragma endregion
};

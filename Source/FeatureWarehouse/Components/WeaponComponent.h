// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

class AWeapon;

UENUM(BlueprintType)
enum class EEquipState : uint8
{
	None UMETA(DisplayName = "None"),
	MainWeapon UMETA(DisplayName = "MainWeapon"),
	SubWeapon UMETA(DisplayName = "SubWeapon")
};

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FEATUREWAREHOUSE_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

	/** 메인 무기를 장착하는 함수 */
	void EquipMainWeapon();

	/** 서브 무기를 장착하는 함수 */
	void EquipSubWeapon();

	void EquipOtherWeapon();

	void Unequip();

	void SaveAcquiredWeaponInfo(AWeapon* NewWeapon);

	UFUNCTION(BlueprintCallable)
	void NotifyToAnimInstance();

	void JumpAttackLanding();

	void Swap();

	void RemoveOwnerWeapon();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	void SaveSingleWeaponInfo(AWeapon* Weapon);

	void SaveMultipleWeaponInfo(AWeapon* Weapon);

	void SaveMainWeaponInfo(AWeapon* Weapon);

	void SaveSubWeaponInfo(AWeapon* Weapon);

	void UpdateWeaponInfoToPlayerAnimInstance();

	void UpdateWeaponInfoToWielderAnimInstance();

	void NotifyHasWeaponToAnim();

private:
	// Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	AWeapon* MainWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	AWeapon* SubWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	int EquipNum;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	class UWielderAnimInstance* WielderAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	class AWielderBase* WielderBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	EEquipState EquipState;

#pragma region GetterSetter
public:
	FORCEINLINE AWeapon* GetMainWeapon() const { return MainWeapon; }
	FORCEINLINE void SetMainWeapon(AWeapon* NewWeapon) { MainWeapon = NewWeapon; }

	FORCEINLINE AWeapon* GetSubWeapon() const { return SubWeapon; }
	FORCEINLINE void SetSubWeapon(AWeapon* NewWeapon) { SubWeapon = NewWeapon; }

	FORCEINLINE EEquipState CurEquipState() { return EquipState; }

	FORCEINLINE int GetEquipNum() { return EquipNum; }

#pragma endregion
};
